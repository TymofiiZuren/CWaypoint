#include "reference_app.hpp"
#include "reference_layout.hpp"
#include "code_editor.hpp"
#include "study_store.hpp"
#include "lab.hpp"
#include "text_input.hpp"
#include "keyboard_input.hpp"
#include "streak_icon.hpp"
#include <raylib.h>
#include <rlgl.h>
#include <mach-o/dyld.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <future>
#include <iostream>
#include <map>
#include <sstream>
#include <thread>
namespace cw {namespace {
const Color bg{17,21,32,255}, card{24,30,45,255}, codeBg{19,24,37,255},
    edge{52,61,83,255}, fg{240,243,252,255}, muted{172,183,205,255},
    accent{190,182,255,255}, primary{94,76,186,255}, selected{49,45,80,255},
    success{120,228,184,255}, errorColor{255,165,167,255}, warm{255,191,121,255};
std::filesystem::path executableFolder(){uint32_t size=4096;std::string path(size,'\0');if(_NSGetExecutablePath(path.data(),&size))return {};path.resize(path.find('\0'));return std::filesystem::weakly_canonical(path).parent_path();}
std::filesystem::path toolFolder(){return executableFolder().parent_path()/"Resources/LabTools";}
std::string initials(const std::string& name){if(name.empty())return "CW";std::string out(1,static_cast<char>(std::toupper(name.front())));auto p=name.find_last_of(' ');if(p!=name.npos && p+1<name.size())out+=static_cast<char>(std::toupper(name[p+1]));return out;}
std::string escaped(std::string value){std::string out;for(char c:value){if(c=='\n')out+="\\n";else out+=c;}return out;}
bool repeat(int key){return IsKeyPressed(key)||IsKeyPressedRepeat(key);}
// Read the actual backing dimensions once; the installed TakeScreenshot path
// applies Retina scaling a second time and exports a padded image.
void captureScreenshot(const char* path){
 int width=GetRenderWidth(),height=GetRenderHeight();Image image{rlReadScreenPixels(width,height),width,height,1,PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
 if(!image.data)throw std::runtime_error("Cannot read the display framebuffer.");
 bool saved=ExportImage(image,path);UnloadImage(image);if(!saved)throw std::runtime_error("Cannot save screenshot.");
}
enum class Page{Learning,Courses,Lesson,Settings,Progress,Review,Compare};
enum LessonStep {Read,Practice,Solo,Summary,Coding};
struct ReferenceApp {
 const AppOptions& options;const std::vector<Lesson>& lessons;Store& store;Controller& app;StudyStore study;
 Rectangle clip{0,0,5000,5000};bool targetSeen=false;
 std::map<std::pair<int,int>,Font> fonts;Texture2D fire{};Page page=Page::Learning;
 int walkthroughStep=0;bool walkthroughVisible=false;
 float scroll=0,extent=0,materialScroll=0,materialExtent=0;
 int lesson=0,question=0;LessonStep stage=Read;
 std::string courseTrack,hint,checkedAnswer;
 bool showNotes=false,showResults=false,showData=false,showHelp=false,resetCode=false,resetProgress=false,exitConfirm=false,closeApproved=false;
 int pendingPage=-1;bool leaving=false;
 Preferences form;std::string search,answer,feedback,notice,focus,action,acted;Verdict verdict=Verdict::Invalid;
 std::map<std::string,TextInput> fields;std::map<std::string,CodeEditor> editors;
 std::map<std::string,float> editorY,editorX;std::vector<std::string> controls,previousControls;
 KeyboardModifiers mods{};bool consumed=false,wheelUsed=false,keyFocus=false,tabNavigation=false;
 int dropdown=0,dropdownChoice=0;Rectangle dropdownAnchor{};std::string dropdownFocus;
 std::future<LabResult> future;std::atomic_bool cancel{false};LabResult result;bool busy=false;
 int resultLesson=-1;unsigned long runGeneration=0;double editedAt=0;bool dirty=false;
 int captureDelay=0;std::string pendingImage;
 size_t testAction=0;int testIdle=0,geometryAttempts=0;std::string testValue;int frames=0;bool testOk=true;
 const std::vector<std::string> testActions{
  "nav:courses","course:C","module:"+lessons[0].id,
  "walk-toggle","walk-next","walk-back","walk-next","walk-toggle",
  "lesson-next","type:answer","check-answer","check-answer","type:answer","check-answer",
  "lesson-next","type:answer","check-answer","lesson-next",
  "run-code","wait-run","assert-run-failed","set-solution","run-code","wait-run",
  "assert-auto-complete","edit-source","assert-stale","editor-options","restore-starter","assert-restore-undo",
  "run-code","wait-run","assert-auto-complete",
  "lesson-next","tab:notes","type:note","tab:learn","exit-lesson",
  "nav:settings","type:name","goal","select:2","theme","select:1","font","select:2",
  "save-preferences","nav:courses","type:search","clear-search","nav:learning"};
 ReferenceApp(const AppOptions& o,const std::vector<Lesson>& l,Store& s,Controller& a):options(o),lessons(l),store(s),app(a),study(s),form(study.preferences){}
 ~ReferenceApp(){cancel.store(true);if(future.valid())future.wait();}
 float pixelScale(){return std::max(1.f,GetRenderWidth()/static_cast<float>(GetScreenWidth()));}
 Font fontAt(float size,bool code=false,bool heavy=false){
  int family=code?2:heavy?1:0,pixels=std::max(12,static_cast<int>(std::round(size*pixelScale())));auto key=std::make_pair(family,pixels);
  auto found=fonts.find(key);if(found!=fonts.end())return found->second;
  const char* paths[]={"/System/Library/Fonts/Supplemental/Arial.ttf","/System/Library/Fonts/Supplemental/Arial Bold.ttf","/System/Library/Fonts/Supplemental/Andale Mono.ttf"};
  auto font=LoadFontEx(paths[family],pixels,nullptr,0);SetTextureFilter(font.texture,TEXTURE_FILTER_BILINEAR);fonts[key]=font;return font;
 }
 float measure(const std::string& s,float size,bool code=false){return MeasureTextEx(fontAt(size,code),s.c_str(),size,code?0.f:.2f).x;}
 void drawText(const std::string& s,Vector2 position,float size,Color color,bool code=false,bool heavy=false){
  float scale=pixelScale();position.x=std::round(position.x*scale)/scale;position.y=std::round(position.y*scale)/scale;
  DrawTextEx(fontAt(size,code,heavy),s.c_str(),position,size,code?0.f:.2f,color);
 }
 void text(const std::string& s,float x,float y,float size=17,Color c=fg,bool heavy=false){drawText(s,{x,y},size,c,false,heavy);}
 float roundness(Rectangle r,float radius=10){return std::min(1.f,2*radius/std::min(r.width,r.height));}
 void surface(Rectangle r,Color fill=card,Color border=edge){DrawRectangleRounded(r,roundness(r),12,fill);DrawRectangleRoundedLinesEx(r,roundness(r),12,1,border);}
 void line(float x,float y,float width){DrawRectangleRec({x,y,width,1},edge);}
 void clipping(Rectangle r){clip=r;BeginScissorMode(r.x,r.y,r.width,r.height);}
 bool inView(Rectangle r){return CheckCollisionRecs(r,clip);}
 bool allowed(const std::string& id){if(leaving)return id.rfind("leave-",0)==0;if(exitConfirm)return id.rfind("close-",0)==0;return !dropdown || id==dropdownFocus || id.rfind("select:",0)==0;}
 bool targeted(const std::string& id){return action==id || action=="type:"+id || (id=="notes"&&action=="type:note");}
 void reveal(Rectangle r,const std::string& id){
  if(!options.referenceTest || !targeted(id))return;targetSeen=true;
  if(r.y<clip.y+4){if(page==Page::Lesson)materialScroll=std::max(0.f,materialScroll+r.y-clip.y-12);else scroll=std::max(0.f,scroll+r.y-clip.y-12);}
  if(r.y+r.height>clip.y+clip.height-4){if(page==Page::Lesson)materialScroll+=r.y+r.height-clip.y-clip.height+12;else scroll+=r.y+r.height-clip.y-clip.height+12;}
 }
 bool hit(Rectangle r,const std::string& id){
  reveal(r,id);if(leaving&&id.rfind("leave-",0)!=0)return false;
  if(!allowed(id)||!inView(r))return false;
  Vector2 point=GetMousePosition();if(options.referenceTest&&targeted(id))point={r.x+r.width/2,r.y+r.height/2};
  return CheckCollisionPointRec(point,r)&&CheckCollisionPointRec(point,clip);
 }
 bool activate(const std::string& id,bool hot,bool enabled){
  if(options.referenceTest){if(!consumed&&hot&&action==id){consumed=true;acted=id;focus=id;return true;}return false;}
  bool pointer=hot&&(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)||action==id);
  bool key=enabled&&focus==id&&keyFocus&&IsKeyPressed(KEY_ENTER)&&allowed(id);
  if(consumed||(!pointer&&!key))return false;
  focus=id;keyFocus=key;consumed=true;acted=id;return true;
 }
 std::pair<Color,Color> buttonColors(bool primaryAction,bool hot,bool chosen){
  if(primaryAction)return {hot?Color{113,93,210,255}:primary,primary};
  if(chosen)return {selected,accent};
  return {hot?selected:Color{33,40,58,255},Color{71,80,103,255}};
 }
 void buttonFace(const std::string& label,Rectangle r,bool main,bool enabled,bool quiet,bool hot,bool active,bool chosen){
  const auto [fill,border]=buttonColors(main&&enabled,hot,chosen);
  if(hot)SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
  if(!quiet)surface(r,fill,border);else if(hot||chosen)surface(r,selected,selected);
  if(active&&keyFocus){Rectangle ring{r.x-3,r.y-3,r.width+6,r.height+6};DrawRectangleRoundedLinesEx(ring,roundness(ring,13),12,2,accent);}
  float fs=16;while(fs>13&&measure(label,fs)>r.width-24)--fs;text(label,r.x+12,r.y+(r.height-fs)/2,fs,enabled?fg:muted);
 }
 bool button(const std::string& id,const std::string& label,Rectangle r,bool main=false,bool enabled=true,bool quiet=false,bool chosen=false){
  if(enabled&&inView(r))controls.push_back(id);
  bool hot=enabled&&hit(r,id);buttonFace(label,r,main,enabled,quiet,hot,focus==id,chosen);
  return activate(id,hot,enabled);
 }
 std::vector<std::string> wrappedRows(const std::string& value,float width,float size){
  std::istringstream input(value);std::string word,row;std::vector<std::string> rows;
  while(input>>word){std::string next=row.empty()?word:row+" "+word;
   if(!row.empty()&&measure(next,size)>width){rows.push_back(row);row=word;}else row=next;
  }if(!row.empty())rows.push_back(row);return rows;
 }
 float paragraphHeight(const std::string& value,float width,float size){return wrappedRows(value,width,size).size()*size*1.55f;}
 float paragraph(const std::string& value,float x,float y,float width,float size=17,Color color=muted){
  for(const auto& row:wrappedRows(value,width,size)){text(row,x,y,size,color);y+=size*1.55f;}return y;
 }
 void autosave(){
  if(!dirty || GetTime()-editedAt<.7)return;
  if(study.save())dirty=false;else editedAt=GetTime();
 }
 void changed(){dirty=true;editedAt=GetTime();}
 void saveEditor(const std::string& id,CodeEditor& editor){
  auto& entry=study.entries[lessons[lesson].id];
  if(id=="notes")entry.note=editor.value();
  else{entry.source=editor.value();entry.cursor=editor.cursor();entry.hasSource=true;}
  changed();
 }
 CodeEditor& code(){
  const auto& id=lessons[lesson].id;auto [it,created]=editors.try_emplace(id);
  if(created){auto& e=study.entries[id];it->second.set(e.hasSource?e.source:labFor(id).starter);it->second.moveTo(e.cursor);}
  return it->second;
 }
 CodeEditor& notes(){auto key=lessons[lesson].id+":notes";auto [it,created]=editors.try_emplace(key);if(created)it->second.set(study.entries[lessons[lesson].id].note);return it->second;}
 bool preferencesChanged(){return form.name!=study.preferences.name||form.goal!=study.preferences.goal||form.editorSize!=study.preferences.editorSize||form.lightEditor!=study.preferences.lightEditor||form.nudge!=study.preferences.nudge;}
 void navigate(Page next){
  if(page==Page::Settings&&preferencesChanged()){pendingPage=static_cast<int>(next);leaving=true;return;}
  if(busy)cancel.store(true);
  if(dirty && !study.save()){notice=study.notice;return;}
  dirty=false;page=next;scroll=0;dropdown=0;courseTrack.clear();focus.clear();showHelp=false;
  if(next==Page::Settings)form=study.preferences;
 }
 void openLesson(int index,bool restore=false){
  if(busy){cancel.store(true);notice="Stopping the current run. Choose the lesson again once it stops.";return;}
  if(dirty && !study.save()){notice=study.notice;return;}
  lesson=index;
  LessonStep target=restore?static_cast<LessonStep>(app.profile.step):Read;
  if(target==Summary&&!app.labPassed(index))target=Coding;
  walkthroughStep=restore?app.profile.walkthrough:0;
  walkthroughVisible=walkthroughStep>0;
  const int savedQuestion=restore?app.profile.question:0;
  page=Page::Lesson;
  showNotes=false;
  setStage(target,savedQuestion);
 }
 int nextInTrack(const std::string& track){for(int i=0;i<static_cast<int>(lessons.size());++i)if(lessons[i].track==track && !app.lessonCompleted(i))return i;for(int i=0;i<static_cast<int>(lessons.size());++i)if(lessons[i].track==track)return i;return 0;}
 int resumeIndex(){for(int i=0;i<static_cast<int>(lessons.size());++i)if(lessons[i].id==app.profile.lesson)return i;return 0;}
 void resume(){openLesson(resumeIndex(),!app.profile.lesson.empty());}
 void logo(float x,float y){
  DrawLineEx({x+34,y},{x+14,y},5,accent);DrawLineEx({x+14,y},{x,y+17},5,accent);DrawLineEx({x,y+17},{x+14,y+34},5,accent);DrawLineEx({x+14,y+34},{x+34,y+34},5,accent);DrawCircleV({x+39,y+3},4,accent);DrawLineEx({x+14,y+18},{x+21,y+25},3,fg);DrawLineEx({x+21,y+25},{x+34,y+9},3,fg);
 }
 void header(){
  clipping({0,0,static_cast<float>(GetScreenWidth()),80});
  DrawRectangle(0,0,GetScreenWidth(),80,bg);line(0,79,GetScreenWidth());
  if(page==Page::Lesson){lessonHeader();return;}
  logo(30,23);text("CWaypoint",88,25,26,fg,true);
  button("nav:learning","My learning",{260,20,128,44},false,true,true,page==Page::Learning);
  button("nav:courses","All courses",{392,20,124,44},false,true,true,page==Page::Courses);
  button("nav:settings","Settings",{520,20,100,44},false,true,true,page==Page::Settings);
  if(acted=="nav:learning")navigate(Page::Learning);if(acted=="nav:courses")navigate(Page::Courses);if(acted=="nav:settings")navigate(Page::Settings);
  const float x=GetScreenWidth()-208;surface({x,16,132,48},Color{36,27,22,255},Color{78,51,34,255});
  const int streak=studyStreak(app.profile.studyDays,localStudyDay());
  DrawTexturePro(fire,{0,0,72,72},{x+12,24,30,32},{0,0},0,WHITE);text(std::to_string(streak)+(streak==1?" day":" days"),x+52,32,17);
  DrawCircle(GetScreenWidth()-42,40,20,Color{48,62,88,255});text(initials(study.preferences.name),GetScreenWidth()-55,33,14);
  if(!options.referenceTest && CheckCollisionPointCircle(GetMousePosition(),{static_cast<float>(GetScreenWidth()-42),40},22) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))navigate(Page::Settings);
 }
 void footer(){
  DrawRectangle(0,GetScreenHeight()-24,GetScreenWidth(),24,bg);
  std::string message=!store.writable()?"Temporary practice: close the other CWaypoint window to save here.":notice.empty()?study.notice:notice;
  text(message,24,GetScreenHeight()-19,13,!store.writable()||study.unsaved?errorColor:muted);
 }
 void heading(const std::string& kicker,const std::string& title,const std::string& subtitle,float x,float y){text(kicker,x,y,13,accent);text(title,x,y+34,34,fg,true);text(subtitle,x,y+86,17,muted);}
 void streakCard(Rectangle r){
  surface(r,Color{27,24,22,255},Color{78,51,34,255});int days=studyStreak(app.profile.studyDays,localStudyDay());
  DrawTexturePro(fire,{0,0,72,72},{r.x+28,r.y+40,58,66},{0,0},0,WHITE);text("LEARNING STREAK",r.x+110,r.y+28,12,warm);
  text(std::to_string(days),r.x+110,r.y+58,36,fg,true);text("day streak",r.x+150,r.y+72,17);
  paragraph(days?"Keep your learning habit going.":"Every great streak starts with one lesson.",r.x+28,r.y+118,r.width-56,15,Color{190,170,153,255});
  const int marks[]={3,7,14,30};float w=(r.width-68)/4;
  for(int i=0;i<4;++i){DrawRectangleRounded({r.x+24+i*(w+6),r.y+166,w,4},1,8,days>=marks[i]?warm:Color{62,46,35,255});text(std::to_string(marks[i])+" days",r.x+25+i*(w+6),r.y+180,12,Color{190,170,153,255});}
  text("Today: "+std::to_string(app.completedToday())+" / "+std::to_string(study.preferences.goal)+" lessons",r.x+26,r.y+214,14,warm);
  if(button("fuel","Continue learning",{r.x+24,r.y+r.height-66,r.width-48,44},true))resume();
 }
 void resumeCard(Rectangle r){
  surface(r);
  const auto& item=lessons[resumeIndex()];
  const bool started=!app.profile.lesson.empty();
  text(item.track+" foundations",r.x+28,r.y+28,15,accent);
  text(item.title,r.x+28,r.y+66,28,fg,true);
  paragraph(item.objective,r.x+28,r.y+115,r.width-56,18,muted);
  if(study.preferences.nudge&&app.completedToday()==0)
   text("You have time for one short lesson today.",r.x+28,r.y+r.height-100,14,muted);
  if(button("continue",started?"Continue lesson":"Start lesson",{r.x+28,r.y+r.height-70,176,44},true))resume();
 }
 void courseCard(const std::string& track,Rectangle r){
  surface(r);surface({r.x+24,r.y+24,50,50},Color{24,39,53,255},Color{24,39,53,255});text(track,r.x+35,r.y+40,20,accent,true);
  text(track+" foundations",r.x+24,r.y+96,22,fg,true);
  paragraph(track=="C"?"Build a foundation with values, pointers and control flow.":"Explore strings, input and the building blocks of C++.",r.x+24,r.y+136,r.width-48,17);
  int count=0,done=0;for(int i=0;i<static_cast<int>(lessons.size());++i)if(lessons[i].track==track){++count;done+=app.lessonCompleted(i);}
  text("Beginner  /  "+std::to_string(count)+(count==1?" module  /  ":" modules  /  ")+std::to_string(done)+" complete  /  "+std::to_string(app.courseProgress(track))+"%",r.x+24,r.y+218,14,muted);
  if(button("course:"+track,"View lessons",{r.x+24,r.y+r.height-62,170,42},true)){courseTrack=track;scroll=0;}
 }
 float savedLessons(float x,float y,float w){
  bool heading=false;for(int i=0;i<static_cast<int>(lessons.size());++i){if(!study.entries[lessons[i].id].bookmark)continue;
   if(!heading){text("Saved lessons",x,y,22,fg,true);y+=46;heading=true;}
   if(button("saved:"+lessons[i].id,lessons[i].title,{x,y,w,52}))openLesson(i);y+=68;
  }return y;
 }
 int catalogueCards(float x,float y,float width,float cardW,bool narrow){int shown=0;
  std::string query=search;std::transform(query.begin(),query.end(),query.begin(),::tolower);
  for(auto track:{std::string("C"),std::string("C++")}){
   std::string name=track+" foundations";std::transform(name.begin(),name.end(),name.begin(),::tolower);
   if(!query.empty() && name.find(query)==name.npos)continue;
   courseCard(track,{x+(narrow?0:shown%2*(cardW+24)),y+(narrow?shown*324:shown/2*324),cardW,300});++shown;
  }
  if(!shown)paragraph("No matching courses. Clear your search to see the available C and C++ courses.",x,y,width,18);
  return shown;
 }
 void home(){
  const float width=760,x=(GetScreenWidth()-width)/2.f,y=128-scroll;
  text("My learning",x,y,40,fg,true);
  resumeCard({x,y+78,width,278});
  extent=savedLessons(x,y+392,width)+scroll+32;
 }
 void coursePath(){
  const float width=760,x=(GetScreenWidth()-width)/2.f;
  float y=116-scroll;
  if(button("back-courses","All courses",{x,y,128,38},false,true,true)){courseTrack.clear();scroll=0;return;}
  y+=58;
  text(courseTrack+" foundations",x,y,38,fg,true);
  y+=64;
  text(std::to_string(app.courseProgress(courseTrack))+"% complete",x,y,17,muted);
  y+=48;
  const int next=nextInTrack(courseTrack);
  for(int i=0;i<static_cast<int>(lessons.size());++i){
   if(lessons[i].track!=courseTrack)continue;
   bool done=app.lessonCompleted(i);
   surface({x,y,width,68});
   text(lessons[i].title,x+22,y+24,18,fg,true);
   const char* label=done?"Review":i==next?"Start lesson":"Open lesson";
   if(button("module:"+lessons[i].id,label,{x+width-150,y+13,130,42},!done&&i==next))openLesson(i);
   y+=80;
  }
  extent=y+scroll+36;
 }
 void catalogue(){
  if(page==Page::Learning){home();return;}
  if(!courseTrack.empty()){coursePath();return;}
  const float width=std::min(1000.f,GetScreenWidth()-96.f),x=(GetScreenWidth()-width)/2,y=128-scroll;
  text("All courses",x,y,40,fg,true);
  textField("search",search,{x+width-320,y,320,44},160,"Search courses...");
  if(!search.empty() && button("clear-search","Clear",{x+width-80,y+50,80,34},false,true,true))search.clear();
  catalogueCards(x,y+108,width,(width-24)/2,false);
  extent=y+scroll+452;
 }
 void clipboard(CodeEditor& editor){
  if(!mods.command && !mods.control)return;
  if(IsKeyPressed(KEY_A))editor.selectAll();
  if(IsKeyPressed(KEY_C) && !editor.selected().empty())SetClipboardText(editor.selected().c_str());
  if(IsKeyPressed(KEY_X) && !editor.selected().empty()){SetClipboardText(editor.selected().c_str());editor.backspace();}
  if(IsKeyPressed(KEY_V)){const char* value=GetClipboardText();if(value && !editor.insert(value))notice="Text must be ASCII and at most 64 KiB.";}
  if(IsKeyPressed(KEY_Z)){if(mods.shift)editor.redo();else editor.undo();}
 }
 void moveEditor(CodeEditor& editor){
  if(repeat(KEY_LEFT)){if(mods.command)editor.home(mods.shift);else editor.move(-1,mods.shift,mods.alt);}
  if(repeat(KEY_RIGHT)){if(mods.command)editor.end(mods.shift);else editor.move(1,mods.shift,mods.alt);}
  if(repeat(KEY_UP))editor.vertical(-1,mods.shift);if(repeat(KEY_DOWN))editor.vertical(1,mods.shift);
  if(repeat(KEY_HOME))editor.moveTo(0,mods.shift);if(repeat(KEY_END))editor.moveTo(editor.value().size(),mods.shift);
 }
 void editCode(CodeEditor& editor,bool source){
  clipboard(editor);moveEditor(editor);
  int c;while((c=GetCharPressed()))if(acceptTextCharacter(c,mods)){if(source)editor.typeCharacter(static_cast<char>(c));else editor.insert(std::string(1,static_cast<char>(c)));}
  if(IsKeyPressed(KEY_ENTER)){if(source)editor.newline();else editor.insert("\n");}
  if(IsKeyPressed(KEY_TAB)&&!tabNavigation)editor.indent(mods.shift);
  if(repeat(KEY_BACKSPACE))editor.backspace();if(repeat(KEY_DELETE))editor.eraseForward();
 }
 void fieldKeys(TextInput& field,size_t max){
  int c;while((c=GetCharPressed()))if(acceptTextCharacter(c,mods) && field.value().size()<max)field.insert(std::string(1,static_cast<char>(c)));
  if(mods.command || mods.control){
   if(IsKeyPressed(KEY_A))field.selectAll();
   if(IsKeyPressed(KEY_V)){const char* t=GetClipboardText();if(t)field.insert(std::string(t).substr(0,max-field.value().size()+field.selected().size()));}
   if(IsKeyPressed(KEY_C))SetClipboardText(field.selected().c_str());
  }
  if(repeat(KEY_BACKSPACE))field.backspace();if(repeat(KEY_DELETE))field.eraseForward();
  if(repeat(KEY_LEFT))field.move(-1,mods.shift,mods.alt);if(repeat(KEY_RIGHT))field.move(1,mods.shift,mods.alt);
 }
 void syntheticField(const std::string& id,TextInput& field,std::string& value,Rectangle r){if(action=="type:"+id&&hit(r,id)){field.set(testValue);value=field.value();focus=id;consumed=true;acted=action;}}
 void textField(const std::string& id,std::string& value,Rectangle r,size_t max=1024,const std::string& placeholder=""){
  controls.push_back(id);auto& field=fields[id];
  if(focus!=id && field.value()!=value)field.set(value);
  if(!options.referenceTest && hit(r,id) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){focus=id;keyFocus=false;field.set(value);field.moveTo(value.size());consumed=true;}
  syntheticField(id,field,value,r);
  if(!options.referenceTest && focus==id && IsWindowFocused() && !dropdown)fieldKeys(field,max);
  value=field.value();surface(r,bg,focus==id?accent:edge);
  fieldContents(id,field,r,placeholder);
 }
 void fieldContents(const std::string& id,TextInput& field,Rectangle r,const std::string& placeholder){
  const auto& value=field.value();if(value.empty()){text(placeholder,r.x+12,r.y+14,16,muted);if(focus==id)DrawRectangleRec({r.x+12,r.y+10,2,24},accent);return;}
  size_t start=0,end=value.size();while(start<field.cursor()&&measure(value.substr(start,field.cursor()-start),16)>r.width-28)++start;
  while(end>start&&measure(value.substr(start,end-start),16)>r.width-28)--end;
  auto outer=clip;clipping({r.x+4,std::max(r.y,outer.y),r.width-8,std::max(1.f,std::min(r.y+r.height,outer.y+outer.height)-std::max(r.y,outer.y))});
  size_t a=std::max(start,field.selectionStart()),b=std::min(end,field.selectionEnd());
  if(b>a)DrawRectangleRec({r.x+12+measure(value.substr(start,a-start),16),r.y+9,measure(value.substr(a,b-a),16),28},selected);
  text(value.substr(start,end-start),r.x+12,r.y+14,16,fg);
  if(focus==id&&static_cast<int>(GetTime()*2)%2==0)DrawRectangleRec({r.x+12+measure(value.substr(start,field.cursor()-start),16),r.y+10,2,24},accent);
  clipping(outer);
 }
 void dropdownField(int kind,const std::string& id,const std::string& value,Rectangle r){
  if(button(id,value,r)){dropdown=dropdown==kind?0:kind;dropdownAnchor=r;dropdownFocus=id;dropdownChoice=0;}
  DrawLineEx({r.x+r.width-28,r.y+20},{r.x+r.width-22,r.y+26},1.5f,muted);DrawLineEx({r.x+r.width-22,r.y+26},{r.x+r.width-16,r.y+20},1.5f,muted);
 }
 std::vector<std::string> dropdownItems(){
  if(dropdown==1)return {"1 lesson - a daily habit","2 lessons - keep momentum","3 lessons - a deeper session"};
  if(dropdown==2)return {"Dark","Light"};
  if(dropdown==4)return {"16 px","18 px","20 px","22 px","24 px"};
  return {"12 px","14 px","16 px","18 px","20 px","22 px","24 px"};
 }
 void selectDropdown(int value){
  if(dropdown==1)form.goal=value+1;
  if(dropdown==2)form.lightEditor=value==1;
  if(dropdown==3)form.editorSize=12+2*value;
  if(dropdown==4)app.setTextSize(16+2*value);
  dropdown=0;focus=dropdownFocus;
 }
 void popup(){
  if(!dropdown)return;if(IsKeyPressed(KEY_ESCAPE)){dropdown=0;consumed=true;return;}clipping({0,80,static_cast<float>(GetScreenWidth()),GetScreenHeight()-104.f});auto rows=dropdownItems();float h=rows.size()*42.f+16;
  Rectangle box{dropdownAnchor.x,std::min(dropdownAnchor.y+52,GetScreenHeight()-32-h),dropdownAnchor.width,h};
  box.y=std::max(84.f,box.y);surface(box);
  if(IsKeyPressed(KEY_DOWN))dropdownChoice=(dropdownChoice+1)%rows.size();
  if(IsKeyPressed(KEY_UP))dropdownChoice=(dropdownChoice+rows.size()-1)%rows.size();
  if(IsKeyPressed(KEY_ENTER)){selectDropdown(dropdownChoice);consumed=true;return;}
  for(size_t i=0;i<rows.size();++i)if(button("select:"+std::to_string(i),rows[i],{box.x+8,box.y+8+i*42,box.width-16,40},false,true,true)){selectDropdown(static_cast<int>(i));break;}
  if(!consumed && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(),box)){dropdown=0;consumed=true;}
 }
 bool savePreferences(){auto before=study.preferences;study.preferences=form;if(study.save()){notice="Preferences saved on this Mac.";return true;}study.preferences=before;notice=study.notice;return false;}
 void closeFailure(){
  if(!exitConfirm)return;clipping({0,80,static_cast<float>(GetScreenWidth()),GetScreenHeight()-104.f});Rectangle r{GetScreenWidth()/2.f-290,220,580,220};surface(r);
  text("Your changes have not been saved",r.x+24,r.y+24,23,errorColor,true);paragraph(study.notice,r.x+24,r.y+70,r.width-48,16);
  if(button("close-retry","Retry save",{r.x+24,r.y+144,150,44},true)){if(study.save())closeApproved=true;}
  if(button("close-discard","Close without saving",{r.x+190,r.y+144,210,44}))closeApproved=true;
  if(button("close-stay","Stay",{r.x+420,r.y+144,130,44}))exitConfirm=false;
 }
 void leaveDialog(){
  if(!leaving)return;clipping({0,80,static_cast<float>(GetScreenWidth()),GetScreenHeight()-104.f});
  Rectangle r{GetScreenWidth()/2.f-290,GetScreenHeight()/2.f-110,580,220};DrawRectangle(0,80,GetScreenWidth(),GetScreenHeight()-104,Color{0,0,0,160});surface(r);
  text("Keep your changes?",r.x+24,r.y+24,25,fg,true);paragraph("Save your preferences before leaving this screen.",r.x+24,r.y+70,r.width-48,17);
  if(button("leave-save","Save and leave",{r.x+24,r.y+144,170,44},true)){if(savePreferences())finishLeaving();}
  if(button("leave-discard","Discard",{r.x+210,r.y+144,150,44})){form=study.preferences;finishLeaving();}
  if(button("leave-stay","Stay",{r.x+378,r.y+144,150,44})){leaving=false;pendingPage=-1;}
 }
 void finishLeaving(){leaving=false;if(pendingPage<0){closeApproved=true;return;}auto next=static_cast<Page>(pendingPage);pendingPage=-1;navigate(next);}
 bool requestClose(){
  if(page==Page::Settings&&preferencesChanged()){leaving=true;pendingPage=-1;return false;}
  if(dirty&&!study.save()){notice=study.notice;exitConfirm=true;return false;}return true;
 }
 float settingsForm(Rectangle r){
  const std::string intro="Make this space feel like yours. Preferences are saved only on this device.";
  const std::string appearance="Applies only to the code editor. The rest of CWaypoint stays dark.";
  const std::string explanation="One completed lesson keeps your streak alive. Your goal is a personal target, not a streak requirement.";
  float w=r.width-56;r.height=774+paragraphHeight(intro,w,17)+paragraphHeight(appearance,w,16)+paragraphHeight(explanation,w,15);
  surface(r);float x=r.x+28,y=r.y+28;text("Profile & learning",x,y,23,fg,true);y+=48;
  y=paragraph(intro,x,y,w,17)+28;
  text("Display name",x,y);y+=30;textField("name",form.name,{x,y,w,46},80,"Your name (optional)");y+=78;
  text("Daily learning goal",x,y);y+=30;dropdownField(1,"goal",std::to_string(form.goal)+" lesson(s) - build a daily habit",{x,y,w,46});y+=78;line(x,y,w);y+=30;
  text("Code editor appearance",x,y,22,fg,true);y+=40;
  y=paragraph(appearance,x,y,w,16)+24;
  text("Theme",x,y);text("Font size",x+w/2+12,y);y+=30;
  dropdownField(2,"theme",form.lightEditor?"Light":"Dark",{x,y,(w-24)/2,46});
  dropdownField(3,"font",std::to_string(form.editorSize)+" px",{x+w/2+12,y,(w-24)/2,46});y+=70;
  surface({x,y,w,64},form.lightEditor?Color{239,241,245,255}:bg);
  drawText("printf(\"Hello, CWaypoint!\\n\");",{x+16,y+22},form.editorSize,form.lightEditor?Color{36,65,126,255}:accent,true);y+=92;
  if(button("nudge",form.nudge?"[x]  Show an in-app daily nudge":"[ ]  Show an in-app daily nudge",{x,y,w,44},false,true,true))form.nudge=!form.nudge;y+=64;
  y=paragraph(explanation,x,y,w,15)+24;
  if(options.referenceTest && y+44>r.y+r.height-28)throw std::runtime_error("Settings actions overflow the profile panel");
  if(button("save-preferences","Save preferences",{x,y,210,44},true))savePreferences();
  if(button("cancel-preferences","Cancel changes",{x+226,y,160,44})){form=study.preferences;fields["name"].set(form.name);notice="Preference changes discarded.";}
  return r.height;
 }
 void settings(){
  float w=std::min(1120.f,GetScreenWidth()-96.f),x=(GetScreenWidth()-w)/2,y=120-scroll;
  heading("YOUR WORKSPACE, YOUR WAY","Settings","Small adjustments for a better learning habit.",x,y);y+=140;
  bool narrow=GetScreenWidth()<1100;float left=narrow?w:(w-24)*.64f;
  float formHeight=settingsForm({x,y,left,0});float sideY=narrow?y+formHeight+24:y;
  streakCard({narrow?x:x+left+24,sideY,narrow?w:w-left-24,296});
  float infoX=narrow?x:x+left+24,infoW=narrow?w:w-left-24;
  surface({infoX,sideY+320,infoW,230});text("Your streak stays with you",infoX+24,sideY+346,21,fg,true);
  paragraph("Complete a lesson each day to keep your flame going. Repeated completions on the same day do not add extra streak days. Nothing syncs across devices.",infoX+24,sideY+390,infoW-48,17);
  y=narrow?sideY+580:y+std::max(formHeight,550.f)+28;
  if(button("learning-data",showData?"Hide learning data and help":"Learning data, reading size & help",{x,y,w,46},false,true,true))showData=!showData;
  y+=66;if(showData)y=dataSettings(x,y,w);extent=y+scroll+40;
 }
 float dataSettings(float x,float y,float w){
  paragraph(app.notice.empty()?store.status():app.notice,x,y,w,16,store.writable()?muted:errorColor);y+=46;
  dropdownField(4,"reading-size",std::to_string(app.profile.textSize)+" px reading size",{x,y,260,44});y+=64;
  if(button("progress","View learning progress",{x,y,230,44}))navigate(Page::Progress);
  if(button("review","Review mistakes",{x+250,y,180,44}))navigate(Page::Review);
  if(button("compare","Compare C and C++",{x+450,y,200,44}))navigate(Page::Compare);y+=64;
  if(button("help","Help & shortcuts",{x,y,210,44}))showHelp=!showHelp;y+=64;
  if(button("reset-progress",resetProgress?"Confirm reset learning progress":"Reset learning progress...",{x,y,310,44})){if(resetProgress)performReset();else resetProgress=true;}
  if(resetProgress && button("cancel-reset","Cancel",{x+330,y,100,44}))resetProgress=false;
  return y+70;
 }
 void performReset(){
  if(busy){cancel.store(true);notice="Stopping the current run. Retry reset once it stops.";return;}
  if(app.reset()){study.syncGeneration();study.save();editors.clear();result={};resultLesson=-1;resetProgress=false;notice="Learning progress reset. Your preferences were kept.";}
 }
 void setStage(LessonStep value,int practiceQuestion=0){
  stage=value;
  question=stage==Solo?2:stage==Practice?std::clamp(practiceQuestion,0,1):0;
  answer.clear();fields["answer"].set("");feedback.clear();hint.clear();checkedAnswer.clear();
  app.retry();materialScroll=0;showHelp=false;resetCode=false;focus.clear();
  app.resume(lesson,stage==Coding?Summary:stage,question,stage==Read?walkthroughStep:0);
 }
 void lessonHeader(){
  if(button("exit-lesson","Exit lesson",{26,20,116,42},false,true,true))navigate(Page::Learning);
  const auto label=lessons[lesson].track+" foundations";
  text(label,(GetScreenWidth()-measure(label,16))/2.f,33,16,muted);
  if(button(showNotes?"tab:learn":"tab:notes",showNotes?"Back to lesson":"My notes",
            {GetScreenWidth()-164.f,20,138,42},false,true,true)){
   showNotes=!showNotes;materialScroll=0;focus.clear();
  }
 }
 void predictionAnswer(const Question& q,float x,float& y,float width){
  if(q.type==QuestionType::Choice){
   for(const auto& choice:q.choices){
    const bool selected=answer==choice;
    if(button("answer-choice:"+choice,choice,{x,y,width,48},false,true,false,selected))answer=choice;
    y+=58;
   }
   return;
  }
  text("Your answer",x,y,15,muted,true);
  y+=26;
  const char* placeholder=q.type==QuestionType::Token?"Type the missing word or symbol":"Type the output or value";
  textField("answer",answer,{x,y,std::min(width,460.f),50},1024,placeholder);
  y+=66;
 }
 void retryProgressSave(float x,float& y,float width){
  if(!app.unsavedChanges||!store.writable())return;
  if(button("retry-progress-save","Retry saving progress",{x,y,width,42},true)){if(app.save())checkAnswer();else feedback=app.notice;}y+=56;
 }
 bool answerEnter(){return !options.referenceTest&&focus=="answer"&&IsKeyPressed(KEY_ENTER)&&!dropdown;}
 bool answeredCorrectly() const{return !feedback.empty()&&answer==checkedAnswer&&verdict==Verdict::Correct;}
 void prediction(float x,float& y,float width){
  if(stage==Solo && !app.issue(lesson,2)){y=paragraph("CWaypoint needs to save your progress on this Mac before you can try this question.",x,y,width,17,errorColor);return;}
  const auto& item=lessons[lesson];
  const auto& q=item.questions[question];
  const auto& context=questionContext(item,q);
  if(!context.empty())exampleCode(context,x,y,width);
  predictionAnswer(q,x,y,width);
  if(!answer.empty()&&!answeredCorrectly()&&answerEnter()){consumed=true;checkAnswer();}
  if(!feedback.empty()&&answer==checkedAnswer)answerFeedback(x,y,width);
  retryProgressSave(x,y,width);
  if(!answeredCorrectly())helpHints(x,y,width);
 }
 void checkAnswer(){
  auto value=app.submitEdited(lesson,question,answer);
  checkedAnswer=answer;feedback=value.message;verdict=value.kind;
  if(answeredCorrectly()){keyFocus=keyFocus||focus=="answer";focus="lesson-next";}
  if(value.kind==Verdict::Correct&&app.lessonCompleted(lesson)&&store.writable()&&!app.unsavedChanges)
   notice="Lesson complete. Progress and streak saved automatically.";
 }
 void answerFeedback(float x,float& y,float width){
  const bool correct=answeredCorrectly();
  const Color ink=correct?success:warm;
  const float height=paragraphHeight(feedback,width-36,16)+28;
  surface({x,y,width,height},correct?Color{23,48,43,255}:Color{50,39,43,255});
  paragraph(feedback,x+18,y+14,width-36,16,ink);
  y+=height+12;
 }
 void helpHints(float x,float& y,float width){
  if(button("hints",showHelp?"Hide hints":"Need a hint?",{x,y,136,38},false,true,true))showHelp=!showHelp;
  y+=46;
  if(!showHelp)return;
  const std::array<const char*,3> labels{"Concept","More guidance","Explanation"};
  for(int i=0;i<3;++i){
   if(button("hint:"+std::to_string(i),labels[i],{x+i*136.f,y,126,38})&&app.help(lesson,question))
    hint=lessons[lesson].questions[question].hints[i];
  }
  y=paragraph(hint,x,y+50,width,16)+8;
 }
 void exampleCode(const std::string& source,float x,float& y,float width){
  std::istringstream input(source);std::string row;
  const float size=15, lineHeight=23;
  const size_t columns=std::max(1.f,(width-32)/measure("M",size,true));
  std::vector<std::string> rows;
  while(std::getline(input,row)){
   if(row.empty())rows.emplace_back();
   for(size_t start=0;start<row.size();start+=columns)
    rows.push_back(row.substr(start,columns));
  }
  surface({x,y,width,24+lineHeight*rows.size()},codeBg);
  y+=12;
  for(const auto& codeRow:rows){
   drawText(codeRow,{x+16,y},size,fg,true);
   y+=lineHeight;
  }
  y+=28;
 }
 void walkthrough(const Lesson& item,float x,float& y,float width){
  walkthroughStep=std::clamp(walkthroughStep,0,static_cast<int>(item.steps.size())-1);
  const auto& step=item.steps[walkthroughStep];
  text("LINE "+std::to_string(walkthroughStep+1)+" / "+std::to_string(item.steps.size()),x,y,13,accent,true);
  y+=24;
  exampleCode(step.code,x,y,width);
  const float stateHeight=paragraphHeight(step.state,width-36,16);
  const float panelHeight=paragraphHeight(step.explanation,width-36,17)+stateHeight+56;
  surface({x,y,width,panelHeight},card);
  text("What changes",x+18,y+14,14,muted,true);
  paragraph(step.state,x+18,y+37,width-36,16,accent);
  paragraph(step.explanation,x+18,y+43+stateHeight,width-36,17,fg);
  y+=panelHeight+14;
  if(button("walk-back","Previous line",{x,y,130,40},false,walkthroughStep>0))moveWalkthrough(-1);
  if(button("walk-next","Next line",{x+width-130,y,130,40},false,walkthroughStep+1<static_cast<int>(item.steps.size())))moveWalkthrough(1);
  y+=54;
 }
 void moveWalkthrough(int change){
  walkthroughStep+=change;
  app.resume(lesson,Read,0,walkthroughStep);
 }
 void lessonExample(const Lesson& item,float x,float& y,float width){
  text("Example",x,y+6,21,fg,true);
  if(!item.steps.empty()&&validateWalkthrough(item.steps).empty()){
   if(button("walk-toggle",walkthroughVisible?"Show full example":"Step through code",{x+width-166,y,166,38})){
    walkthroughVisible=!walkthroughVisible;
   }
  }
  y+=50;
  if(walkthroughVisible)walkthrough(item,x,y,width);
  else exampleCode(item.code,x,y,width);
 }
 void editorPointer(CodeEditor& editor,Rectangle r,float size,const std::string& id){
  if(options.referenceTest||!hit(r,id))return;
  if(!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&&!(focus==id&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)))return;
  focus=id;keyFocus=false;consumed=true;auto rows=editor.lines();
  size_t row=std::clamp(static_cast<int>((GetMouseY()-r.y-16+editorY[id])/(size*1.6f)),0,static_cast<int>(rows.size())-1);
  size_t col=std::max(0,static_cast<int>((GetMouseX()-r.x-48+editorX[id])/measure("M",size,true)));
  size_t offset=0;for(size_t i=0;i<row;++i)offset+=rows[i].size()+1;
  editor.moveTo(offset+std::min(col,rows[row].size()),mods.shift||!IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
 }
 Color syntaxColor(SyntaxKind kind,Color plain){
  const Color dark[]={fg,{195,157,255,255},{110,190,245,255},{245,190,126,255},{152,209,161,255},{139,155,145,255},{135,171,235,255},{232,214,155,255}};
  const Color light[]={{35,43,57,255},{121,52,165,255},{17,91,151,255},{151,78,15,255},{35,111,63,255},{89,107,91,255},{58,89,155,255},{123,79,23,255}};
  if(kind==SyntaxKind::Plain)return plain;return (study.preferences.lightEditor?light:dark)[static_cast<int>(kind)];
 }
 void syntaxRow(const std::string& row,size_t offset,const std::vector<SyntaxToken>& tokens,Vector2 position,float size,Color ink,bool source){
  if(!source){drawText(row,position,size,ink,true);return;}
  float cell=measure("M",size,true);
  for(const auto& token:tokens){
   size_t a=std::max(offset,token.start),b=std::min(offset+row.size(),token.start+token.length);if(b<=a)continue;
   drawText(row.substr(a-offset,b-a),{position.x+(a-offset)*cell,position.y},size,syntaxColor(token.kind,ink),true);
  }
 }
 void editorRows(CodeEditor& editor,Rectangle r,float size,const std::string& id,bool source){
  float y=r.y+16-editorY[id],cell=measure("M",size,true);size_t offset=0;
  Color ink=source&&study.preferences.lightEditor?Color{35,43,57,255}:fg;
  auto tokens=source?lexCxx(editor.value()):std::vector<SyntaxToken>{};
  for(const auto& row:editor.lines()){
   if(y+size>=r.y && y<r.y+r.height){
    text(std::to_string(1+std::count(editor.value().begin(),editor.value().begin()+offset,'\n')),r.x+10,y,size,muted);
    size_t a=std::max(offset,editor.selectionStart()),b=std::min(offset+row.size(),editor.selectionEnd());
    if(b>a)DrawRectangleRec({r.x+48+(a-offset)*cell-editorX[id],y,(b-a)*cell,size*1.6f},source&&study.preferences.lightEditor?Color{192,213,245,255}:selected);
    syntaxRow(row,offset,tokens,{r.x+48-editorX[id],y},size,ink,source);
    if(focus==id && editor.cursor()>=offset && editor.cursor()<=offset+row.size())DrawRectangleRec({r.x+48+(editor.cursor()-offset)*cell-editorX[id],y,2,size+2},accent);
   }offset+=row.size()+1;y+=size*1.6f;
  }
 }
 void followCaret(CodeEditor& editor,Rectangle r,float size,const std::string& id){
  float cy=editor.line()*size*1.6f;auto p=editor.value().rfind('\n',editor.cursor()?editor.cursor()-1:0);size_t col=editor.cursor()-(p==std::string::npos?0:p+1);
  float cx=col*measure("M",size,true);
  if(cy<editorY[id])editorY[id]=cy;if(cy>editorY[id]+r.height-48)editorY[id]=cy-r.height+48;
  if(cx<editorX[id])editorX[id]=cx;if(cx>editorX[id]+r.width-72)editorX[id]=cx-r.width+72;
 }
 void editorWheel(CodeEditor& editor,Rectangle r,float size,const std::string& id){
  if(!hit(r,id)||!GetMouseWheelMove())return;wheelUsed=true;
  if(mods.shift)editorX[id]=std::max(0.f,editorX[id]-GetMouseWheelMove()*size*3);
  else editorY[id]=std::clamp(editorY[id]-GetMouseWheelMove()*size*3,0.f,std::max(0.f,editor.lines().size()*size*1.6f-r.height+32));
 }
 void editorWidget(const std::string& id,CodeEditor& editor,Rectangle r,float size,bool source){
  if(inView(r))controls.push_back(id);surface(r,source&&study.preferences.lightEditor?Color{239,241,245,255}:codeBg);
  editorPointer(editor,r,size,id);auto before=editor.value();
  if(action=="type:note" && id=="notes" && hit(r,id)){editor.set(testValue);acted=action;consumed=true;}
  if(!options.referenceTest && focus==id && IsWindowFocused() && !dropdown){auto cursor=editor.cursor();editCode(editor,source);if(cursor!=editor.cursor())followCaret(editor,r,size,id);}
  if(before!=editor.value())saveEditor(id,editor);
  editorWheel(editor,r,size,id);
  auto outer=clip;clipping({std::max(r.x+1,outer.x),std::max(r.y+1,outer.y),r.width-2,std::max(1.f,std::min(r.y+r.height,outer.y+outer.height)-std::max(r.y+1,outer.y))});editorRows(editor,r,size,id,source);clipping(outer);
 }
 bool currentResult(){return resultLesson==lesson && result.sourceHash==sourceHash(code().value()) && runGeneration==store.activeGeneration();}
 void startRun(){
  if(busy)return;
  auto exercise=labFor(lessons[lesson].id);auto source=code().value();cancel.store(false);busy=true;showResults=false;resultLesson=lesson;runGeneration=store.activeGeneration();result={};
  future=std::async(std::launch::async,[this,exercise,source]{return runLab(exercise,source,executableFolder()/"CWaypointLabHelper",toolFolder(),&cancel);});
 }
 void pollRun(){
  if(!busy || future.wait_for(std::chrono::milliseconds(0))!=std::future_status::ready)return;
  try{result=future.get();}catch(const std::exception& e){result={};result.diagnostics=e.what();}
  busy=false;notice.clear();
  if(currentResult()&&result.allPassed())complete();
 }
 void complete(){
  if(!currentResult()||!result.allPassed()){notice="Run this version and pass every test first.";return;}
  if(!study.saveSnapshot(result.sourceHash,code().value())){notice=study.notice;return;}
  if(app.recordLabResult(lesson,result,runGeneration))notice=app.lessonCompleted(lesson)?"Lesson complete. Progress and streak saved automatically.":"Coding exercise passed and saved. Finish both knowledge checks to complete the lesson.";
  else notice="Could not save the coding result. "+app.notice;
 }
 void testLabAction(){
  if(action=="assert-run-failed"){if(!result.compiled||result.allPassed()||app.labPassed(lesson))throw std::runtime_error("Starter code must fail without earning coding credit");acted=action;}
  if(action=="edit-source"){code().insert("\n// Another revision\n");saveEditor("source",code());acted=action;}
  if(action=="assert-auto-complete"){if(!app.lessonCompleted(lesson)||!app.completedToday())throw std::runtime_error("Passing lab did not save completion automatically");acted=action;}
  if(action=="assert-stale"){if(currentResult())throw std::runtime_error("Edited source reused a stale result");acted=action;}
  if(action=="set-solution"){code().set(labFor(lessons[lesson].id).solution);saveEditor("source",code());acted=action;}
  if(action=="assert-restore-undo")testRestoreUndo();
 }
 void testRestoreUndo(){
  const auto exercise=labFor(lessons[lesson].id);
  auto& editor=code();
  if(editor.value()!=exercise.starter||!editor.undo()||editor.value().find("// Another revision")==std::string::npos)
   throw std::runtime_error("Restoring starter code must be undoable");
  editor.moveTo(0);saveEditor("source",editor);acted=action;
 }
 void editorOptions(float x,float& y,float width){
  if(button("editor-options",resetCode?"Close options":"Editor options",{x+width-138,y,138,36},false,!busy,true))
   resetCode=!resetCode;
  y+=44;
  if(!resetCode)return;
  surface({x,y,width,86},card);
  paragraph("Restore the original starter code. Press Command-Z to undo.",x+16,y+14,width-32,15,muted);
  if(button("restore-starter","Restore starter code",{x+16,y+41,174,34},false,!busy)){
   auto& editor=code();
   editor.selectAll();
   if(editor.insert(labFor(lessons[lesson].id).starter)){
    editor.moveTo(0);editorX["source"]=editorY["source"]=0;
    saveEditor("source",editor);
    resetCode=false;focus="source";
   }
  }
  y+=100;
 }
 float testCaseResult(const ProcessResult& run,const std::string& expected,size_t index,float x,float y,float width){
  bool passed=run.exitCode==0&&!run.timedOut&&!run.outputLimited&&!run.cancelled&&run.output==expected;
  std::string status=run.cancelled?"Stopped":passed?"Passed":"Failed";
  y=paragraph("Test "+std::to_string(index+1)+"   "+status,x,y,width,15,passed?success:errorColor)+4;
  y=paragraph("Your output: "+escaped(run.output),x,y,width,14,fg)+4;
  if(run.timedOut)y=paragraph("Time limit reached.",x,y,width,15,errorColor);
  if(run.outputLimited)y=paragraph("Output limit reached.",x,y,width,15,errorColor);
  if(!run.error.empty())y=paragraph(run.error.substr(0,1200),x,y,width,14,errorColor)+8;
  return paragraph("Expected: "+escaped(expected),x,y,width,14,muted)+12;
 }
 void showTestResults(float x,float& y,float width){
  if(resultLesson!=lesson)return;
  text("Test results",x,y,21,fg,true);
  y+=34;
  if(busy){y=paragraph("Running tests...",x,y,width,17,accent);return;}
  if(result.cancelled){y=paragraph("Run stopped.",x,y,width,17,warm);return;}
  if(!currentResult())y=paragraph("Code changed. Run the tests again to update these results.",x,y,width,16,warm)+8;
  const auto exercise=labFor(lessons[lesson].id);
  if(!result.diagnostics.empty())y=paragraph(result.diagnostics.substr(0,1600),x,y,width,15,errorColor)+12;
  if(result.runs.empty())return;
  const std::string summary=std::to_string(result.passed)+" of "+std::to_string(exercise.cases.size())+" tests passed";
  y=paragraph(summary,x,y,width,17,result.allPassed()?success:fg)+10;
  if(button("test-details",showResults?"Hide test details":"Show test details",{x,y,158,40}))showResults=!showResults;
  y+=52;
  if(!showResults)return;
  for(size_t i=0;i<result.runs.size();++i){
   y=testCaseResult(result.runs[i],exercise.cases[i].expected,i,x,y,width);
  }
 }
 void codingTask(float x,float& y,float width){
  const auto exercise=labFor(lessons[lesson].id);
  y=paragraph(exercise.instructions,x,y,width,19,fg)+20;
  text(lessons[lesson].track=="C"?"main.c":"main.cpp",x,y+11,16,muted,true);
  editorOptions(x,y,width);
  const float editorHeight=std::clamp(GetScreenHeight()-450.f,188.f,340.f);
  editorWidget("source",code(),{x,y,width,editorHeight},study.preferences.editorSize,true);
  y+=editorHeight+24;
  showTestResults(x,y,width);
  testLabAction();
 }
 void lessonSummary(float x,float& y,float width){
  const std::array<const char*,3> labels{"First knowledge check","Second knowledge check","Coding task"};
  const std::array<bool,3> done{app.solved(lesson,0),app.solved(lesson,1),app.labPassed(lesson)};
  for(int i=0;i<3;++i){
   surface({x,y,width,52});
   text(labels[i],x+16,y+17,16,fg);
   text(done[i]?"Done":"To do",x+width-76,y+17,15,done[i]?success:muted);
   y+=62;
  }
  y+=12;
  if(button("solo","Try another question",{x,y,184,42}))setStage(Solo);
  y+=56;
 }
 int phase() const{
  if(stage==Read)return 0;
  if(stage==Practice||stage==Solo)return 1;
  return stage==Coding?2:3;
 }
 std::string stepTitle() const{
  if(stage==Read)return lessons[lesson].title;
  if(stage==Practice||stage==Solo)return lessons[lesson].questions[question].prompt;
  if(stage==Coding)return "Write and test your code";
  return app.lessonCompleted(lesson)?"Lesson complete":"Lesson summary";
 }
 void stepHeading(float x,float& y,float width){
  const std::array<const char*,4> phases{"Learn","Practice","Code","Review"};
  surface({x,y,width,42},codeBg);
  const float cell=width/4;
  for(int i=0;i<4;++i){
   const bool active=i==phase();
   const float left=x+i*cell;
   if(active)surface({left+4,y+4,cell-8,34},selected,selected);
   const float labelWidth=measure(phases[i],15);
   text(phases[i],left+(cell-labelWidth)/2,y+12,15,active?accent:muted,active);
  }
  y+=66;
  if(stage==Practice||stage==Solo){
   const auto label=stage==Solo?"Extra practice":"Question "+std::to_string(question+1)+" of 2";
   text(label,x,y,15,accent,true);
   y+=28;
  }
  for(const auto& row:wrappedRows(stepTitle(),width,32)){text(row,x,y,32,fg,true);y+=44;}
  y+=10;
 }
 void lessonBody(Rectangle r){
  clipping(r);
  float x=r.x,width=r.width,y=r.y+30-materialScroll;
  if(showNotes){
   text("Your lesson notes",x,y,32,fg,true);
   y=paragraph("Notes are private and saved on this Mac.",x,y+52,width,17)+14;
   editorWidget("notes",notes(),{x,y,width,std::max(180.f,r.height-150)},study.preferences.editorSize,false);
   materialExtent=0;
   return;
  }
  stepHeading(x,y,width);
  switch(stage){
   case Read:
    y=paragraph(lessons[lesson].body,x,y,width,app.profile.textSize+1,fg)+22;
    lessonExample(lessons[lesson],x,y,width);
    break;
   case Practice:
   case Solo:
    prediction(x,y,width);
    break;
   case Coding:
    codingTask(x,y,width);
    break;
   case Summary:
    lessonSummary(x,y,width);
    break;
  }
  materialExtent=std::max(0.f,y+materialScroll-(r.y+r.height)+24);
 }
 void previousLessonStep(){
  if(stage==Practice){setStage(question==0?Read:Practice);return;}
  if(stage==Coding){setStage(Practice,1);return;}
  if(stage==Summary){setStage(Coding);return;}
  if(stage==Solo)setStage(Summary);
 }
 void nextLessonStep(){
  if(stage==Read){setStage(Practice);return;}
  if(stage==Practice){if(question==0)setStage(Practice,1);else setStage(Coding);return;}
  if(stage==Coding||stage==Solo){setStage(Summary);return;}
  if(!app.solved(lesson,0)){setStage(Practice);return;}
  if(!app.solved(lesson,1)){setStage(Practice,1);return;}
  if(!app.labPassed(lesson)){setStage(Coding);return;}
  const int next=nextInTrack(lessons[lesson].track);
  if(!app.lessonCompleted(next)){openLesson(next);return;}
  const auto track=lessons[lesson].track;
  navigate(Page::Courses);
  if(page==Page::Courses)courseTrack=track;
 }
 std::string nextLessonLabel(){
  if(stage==Read)return "Start questions";
  if(stage==Practice)return question==0?"Next question":"Go to coding task";
  if(stage==Solo)return "Back to summary";
  if(stage==Coding)return "View summary";
  if(!app.lessonCompleted(lesson))return "Finish remaining tasks";
  return app.courseProgress(lessons[lesson].track)==100?"View course":"Next lesson";
 }
 void codingFeedback(float x,float y){
  if(busy){text("Running tests...",x+100,y+14,15,muted);return;}
  if(resultLesson!=lesson)return;
  const bool current=currentResult();
  const bool passed=current&&result.allPassed();
  if(passed&&!app.labPassed(lesson)){
   if(button("retry-result","Save result again",{x+100,y,148,44}))complete();
   return;
  }
  const char* status=result.cancelled?"Run stopped":!current?"Edited since last run":passed?"All tests passed":"Check test results";
  text(status,x+100,y+14,15,passed?success:muted);
  if(button("jump-results","View results",{x+265,y,116,44},false,true,true))materialScroll=materialExtent;
 }
 void codingFooter(float x,float y,float width){
  const bool passed=app.labPassed(lesson)&&!busy;
  if(button("run-code",busy?"Stop run":passed?"Run again":"Run tests",{x+width-148,y,148,44},!passed)){
   if(busy)cancel.store(true);else startRun();
  }
  codingFeedback(x,y);
  if(passed&&button("lesson-next","View summary",{x+width-302,y,144,44},true))nextLessonStep();
 }
 void questionFooter(Rectangle r,float y){
  const float right=r.x+r.width;
  if(!answeredCorrectly()){
   if(button("check-answer","Check answer",{right-158,y,158,44},true,!answer.empty()))checkAnswer();
   const bool solved=app.solved(lesson,question);
   const char* label=stage==Solo?"Back to summary":solved?"Continue":"Skip for now";
   if(button("lesson-next",label,{right-318,y,144,44},false,true,true))nextLessonStep();
   return;
  }
  if(button("lesson-next",nextLessonLabel(),{right-206,y,206,44},true))nextLessonStep();
 }
 void lessonFooter(Rectangle r){
  clipping(r);DrawRectangleRec(r,bg);line(r.x,r.y,r.width);
  const float y=r.y+10;
  if(showNotes){
   if(button("notes-done","Done",{r.x+r.width-116,y,116,44},true)){showNotes=false;focus.clear();}
   return;
  }
  if(stage!=Read&&button("lesson-back","Back",{r.x,y,88,44},false,true,true))previousLessonStep();
  if(stage==Coding){codingFooter(r.x,y,r.width);return;}
  if(stage==Practice||stage==Solo){questionFooter(r,y);return;}
  if(button("lesson-next",nextLessonLabel(),{r.x+r.width-206,y,206,44},true))nextLessonStep();
 }
 void lessonScreen(){
  const auto layout=referenceLayout(GetScreenWidth(),GetScreenHeight());
  const float bodyHeight=layout.bodyBottom-layout.bodyTop;
  lessonBody({layout.x,layout.bodyTop,layout.width,bodyHeight});
  EndScissorMode();
  scrollbar(layout.x+layout.width+22,layout.bodyTop+12,bodyHeight-24,materialScroll,bodyHeight-24+materialExtent);
  lessonFooter({layout.x,layout.bodyBottom,layout.width,layout.footerHeight});
 }
 void compareContent(float x,float& y,float w){
  for(const auto& c:comparisons()){
   text(c.title,x,y,24,fg,true);y+=44;y=paragraph(c.explanation,x,y,w,17)+24;
   for(const auto& pair:{std::make_pair("C",c.c),std::make_pair("C++",c.cpp)}){text(pair.first,x,y,16,accent);y+=30;std::istringstream input(pair.second);std::string row;while(std::getline(input,row)){drawText(row,{x,y},15,fg,true);y+=26;}y+=24;}
  }
 }
 void reviewContent(float x,float& y,float w){
  auto items=app.reviewItems();if(items.empty()){y=paragraph("No mistakes to review yet. Practice a lesson to build your review list.",x,y,w,18)+30;return;}
  for(const auto& item:items){text(lessons[item.lesson].title,x,y,23,fg,true);y+=40;y=paragraph(app.status(item.lesson,item.question),x,y,w,17)+16;
   if(button("review-item:"+std::to_string(item.lesson)+":"+std::to_string(item.question),"Review question",{x,y,190,44})){openLesson(item.lesson);setStage(item.question==2?Solo:Practice,item.question);}
   y+=72;
  }
 }
 void learningData(){
  float w=std::min(1000.f,GetScreenWidth()-96.f),x=(GetScreenWidth()-w)/2,y=120-scroll;
  heading("YOUR LEARNING JOURNEY",page==Page::Progress?"Progress":page==Page::Review?"Review mistakes":"C and C++","Practice evidence and completed labs are tracked separately.",x,y);y+=150;
  if(page==Page::Compare){compareContent(x,y,w);extent=y+scroll+40;return;}
  if(page==Page::Review){reviewContent(x,y,w);extent=y+scroll+40;return;}
  for(int i=0;i<static_cast<int>(lessons.size());++i){
   surface({x,y,w,118});text(lessons[i].title,x+24,y+20,22,fg,true);
   text(std::to_string(app.completed(i))+" / 2 checks solved  |  "+(app.labPassed(i)?"Lab passed":"Lab not passed")+"  |  "+std::to_string(app.learningUnits(i))+" / 3 tasks",x+24,y+58,16,muted);
   if(button("data:"+lessons[i].id,"Open lesson",{x+w-174,y+38,150,42}))openLesson(i);y+=142;
  }extent=y+scroll+40;
 }
 void helpOverlay(){
  if(!showHelp||page==Page::Lesson)return;
  Rectangle r{GetScreenWidth()/2.f-330,150,660,310};surface(r);
  text("Help & shortcuts",r.x+24,r.y+24,25,fg,true);
  paragraph("Tab moves between controls. Enter activates a focused button. In the editor: Command-A selects all, Command-C/V copy and paste, Command-Z undoes, Shift-Command-Z redoes. Tab indents code. Scroll inside the editor to move through code. Learner programs run offline with bounded memory and execution time.",r.x+24,r.y+70,r.width-48,17,fg);
  if(button("close-help","Close",{r.x+24,r.y+242,120,44},true))showHelp=false;
 }
 void keyboard(){
  if(options.referenceTest)return;
  if(IsKeyPressed(KEY_ESCAPE)){focus.clear();keyFocus=true;}
  if(!IsKeyPressed(KEY_TAB)||focus=="source"||focus=="notes")return;
  if(previousControls.empty())return;
  auto it=std::find(previousControls.begin(),previousControls.end(),focus);int index=it==previousControls.end()?-1:it-previousControls.begin();
  index=(index+(mods.shift?-1:1)+previousControls.size())%previousControls.size();focus=previousControls[index];keyFocus=true;tabNavigation=true;
 }
 void scrollPage(){
  if(wheelUsed)return;float wheel=GetMouseWheelMove();if(!wheel)return;
  if(page==Page::Lesson)materialScroll=std::clamp(materialScroll-wheel*42,0.f,materialExtent);
  else scroll=std::clamp(scroll-wheel*48,0.f,std::max(0.f,extent-GetScreenHeight()+24));
 }
 void scrollbar(float x,float top,float height,float offset,float total){
  if(total<=height)return;float thumb=std::max(24.f,height*height/total),position=offset/(total-height)*(height-thumb);
  DrawRectangleRounded({x,top+position,4,thumb},1,8,Color{85,94,111,255});
 }
 void draw(){
  controls.clear();consumed=false;tabNavigation=false;acted.clear();wheelUsed=false;mods=currentKeyboardModifiers();keyboard();SetMouseCursor(MOUSE_CURSOR_DEFAULT);
  clipping({0,80,static_cast<float>(GetScreenWidth()),GetScreenHeight()-104.f});
  if(page==Page::Learning||page==Page::Courses)catalogue();else if(page==Page::Settings)settings();else if(page==Page::Lesson)lessonScreen();else learningData();
  EndScissorMode();if(page!=Page::Lesson)scrollbar(GetScreenWidth()-8,88,GetScreenHeight()-120,scroll,extent-80);header();popup();EndScissorMode();helpOverlay();leaveDialog();closeFailure();EndScissorMode();footer();previousControls=controls;scrollPage();
 }
 void testInputValue(){testValue=question==0?"5":"int";
  if(testAction==9)testValue="incorrect";
  if(action=="type:note")testValue="My saved lesson note.";
  if(action=="type:name")testValue="Tymofii";
  if(action=="type:search")testValue="C++";
 }
 bool testGeometry(){
  if(options.compact||GetScreenWidth()>=1280)return true;
  if(++geometryAttempts>8)throw std::runtime_error("Wide reference test requires a window at least 1280 px wide.");
  SetWindowSize(1600,1000);return false;
 }
 void driveTest(){
  action.clear();if(!options.referenceTest||frames%8)return;
  if(!testGeometry())return;
  if(testAction>=testActions.size())return;
  action=testActions[testAction];testInputValue();
  if(action=="wait-run"){if(!busy){++testAction;testIdle=0;}action.clear();return;}
  if(++testIdle>180){std::cerr<<"Reference failure: step="<<testAction<<" width="<<GetScreenWidth()<<" stage="<<stage<<" practice="<<app.completed(lesson)<<" compiled="<<result.compiled<<" passed="<<result.passed<<" current="<<currentResult()<<" diagnostics="<<result.diagnostics<<'\n';captureScreenshot((std::filesystem::path(options.profile)/"failed-reference.png").c_str());throw std::runtime_error("Reference journey could not activate "+action);}
 }
 void finishTestFrame(){
  if(action.empty()||acted!=action)return;
  if(!options.capture.empty()){
   if(action=="save-preferences")pendingImage="settings-footer.png";
   if(action=="assert-auto-complete")pendingImage="highlighted-editor.png";
   if(action=="lesson-next"&&stage==Practice)pendingImage=question==0?"knowledge-check.png":"missing-type.png";
   captureDelay=4;
  }
  ++testAction;testIdle=0;
 }
 void captureFrame(){if(pendingImage.empty()||--captureDelay>0)return;captureScreenshot((std::filesystem::path(options.capture)/pendingImage).c_str());pendingImage.clear();}
 bool verifyTest(){
  if(!options.referenceTest||testAction<testActions.size())return false;
  auto& e=study.entries[lessons[0].id];
  if(!app.lessonCompleted(0)||e.note!="My saved lesson note."||study.preferences.name!="Tymofii"||study.preferences.goal!=3||study.preferences.editorSize!=16||!study.preferences.lightEditor)throw std::runtime_error("Reference journey persistence assertion failed");
  if(app.profile.evidence.at(lessons[0].questions[0].id).attempts!=2)throw std::runtime_error("Changed answers must record twice; duplicate clicks once");
  StudyStore restored(store);if(restored.preferences.name!="Tymofii"||restored.entries.at(lessons[0].id).note!="My saved lesson note."||store.load().achievements.empty())throw std::runtime_error("Reference saved state reload failed");
  std::cout<<"Reference screen journey: PASS\n";return true;
 }
 void assets(){
  auto image=LoadImageFromMemory(".png",streakFirePng,sizeof(streakFirePng));fire=LoadTextureFromImage(image);UnloadImage(image);

 }
 void unload(){if(busy){cancel.store(true);future.wait();}study.save();UnloadTexture(fire);for(auto& entry:fonts)UnloadFont(entry.second);}
};
std::pair<int,int> initialWindowSize(const AppOptions& options){
 if(options.compact)return {1000,740};
 if(options.referenceTest)return {1600,1000};
 return {1280,780};
}
} // namespace
int checkPackagedRunner(){auto exercise=labFor("c-variables");auto result=runLab(exercise,exercise.solution,executableFolder()/"CWaypointLabHelper",toolFolder());std::cout<<"Packaged runner: "<<(result.allPassed()?"PASS":"FAIL")<<'\n';return result.allPassed()?0:1;}
int runReferenceApp(const AppOptions& options,const std::vector<Lesson>& lessons,Store& store,Controller& controller){
 const auto [width,height]=initialWindowSize(options);
 SetConfigFlags(FLAG_WINDOW_RESIZABLE|FLAG_WINDOW_HIGHDPI|FLAG_MSAA_4X_HINT);InitWindow(width,height,"CWaypoint");SetWindowMinSize(960,640);SetExitKey(KEY_NULL);SetTargetFPS(60);
 ReferenceApp ui(options,lessons,store,controller);ui.assets();
 if(options.referenceTest||!options.capture.empty())std::cout<<"Display: logical="<<GetScreenWidth()<<"x"<<GetScreenHeight()<<" framebuffer="<<GetRenderWidth()<<"x"<<GetRenderHeight()<<" scale="<<ui.pixelScale()<<"\n";controller.enableLabMode();
 if(options.initial=="lesson")ui.resume();
 const std::map<std::string,Page> screens{{"settings",Page::Settings},{"progress",Page::Progress},{"review",Page::Review},{"compare",Page::Compare}};
 auto initial=screens.find(options.initial);if(initial!=screens.end())ui.navigate(initial->second);
 while(!ui.closeApproved){
  if(WindowShouldClose()&&ui.requestClose())break;
  ui.pollRun();ui.autosave();ui.driveTest();BeginDrawing();ClearBackground(bg);ui.draw();EndDrawing();ui.finishTestFrame();ui.captureFrame();++ui.frames;
  if(ui.verifyTest()){if(!options.capture.empty())captureScreenshot((std::filesystem::path(options.capture)/"reference-journey.png").c_str());break;}
  if(!options.referenceTest&&!options.capture.empty()&&ui.frames==15){captureScreenshot((std::filesystem::path(options.capture)/"reference.png").c_str());break;}
 }
 ui.unload();CloseWindow();return 0;
}
} // namespace cw
