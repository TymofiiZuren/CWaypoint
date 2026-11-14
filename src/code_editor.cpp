#include "code_editor.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
namespace cw {namespace {
bool validText(const std::string& s){
 if(s.size()>65536)return false;
 for(unsigned char c:s)if(c!='\n' && c!='\t' && (c<32 || c>126))return false;
 return true;
}
size_t startOfLine(const std::string& s,size_t p){if(!p)return 0;auto n=s.rfind('\n',p-1);return n==s.npos ? 0:n+1;}
size_t endOfLine(const std::string& s,size_t p){auto n=s.find('\n',p);return n==s.npos ? s.size():n;}
bool wordChar(unsigned char c){return std::isalnum(c) || c=='_';}
}
size_t CodeEditor::selectionStart()const{return std::min(state.caret,state.anchor);}
size_t CodeEditor::selectionEnd()const{return std::max(state.caret,state.anchor);}
std::string CodeEditor::selected()const{return state.text.substr(selectionStart(),selectionEnd()-selectionStart());}
void CodeEditor::remember(){past.push_back(state);if(past.size()>24)past.pop_front();future.clear();}
bool CodeEditor::set(const std::string& s){if(!validText(s))return false;state={s,0,0};past.clear();future.clear();return true;}
void CodeEditor::replace(size_t a,size_t b,const std::string& s){remember();state.text.replace(a,b-a,s);state.caret=state.anchor=a+s.size();}
bool CodeEditor::insert(const std::string& s){
 if(!validText(s) || state.text.size()-selected().size()+s.size()>65536)return false;
 replace(selectionStart(),selectionEnd(),s);return true;
}
void CodeEditor::moveTo(size_t p,bool extend){state.caret=std::min(p,state.text.size());if(!extend)state.anchor=state.caret;}
void CodeEditor::move(int direction,bool extend,bool word){
 auto p=state.caret;
 if(!extend && selectionStart()!=selectionEnd()){moveTo(direction<0?selectionStart():selectionEnd());return;}
 if(direction<0 && p){--p;if(word)while(p && wordChar(state.text[p-1]))--p;}
 if(direction>0 && p<state.text.size()){++p;if(word)while(p<state.text.size() && wordChar(state.text[p]))++p;}
 moveTo(p,extend);
}
void CodeEditor::home(bool extend){moveTo(startOfLine(state.text,state.caret),extend);}
void CodeEditor::end(bool extend){moveTo(endOfLine(state.text,state.caret),extend);}
void CodeEditor::vertical(int direction,bool extend){
 const auto start=startOfLine(state.text,state.caret),column=state.caret-start;
 if(direction<0){if(!start){moveTo(0,extend);return;}auto prev=startOfLine(state.text,start-1);moveTo(std::min(prev+column,start-1),extend);return;}
 auto next=endOfLine(state.text,state.caret);if(next<state.text.size())++next;
 moveTo(std::min(next+column,endOfLine(state.text,next)),extend);
}
void CodeEditor::selectAll(){state.anchor=0;state.caret=state.text.size();}
void CodeEditor::backspace(){if(selectionStart()!=selectionEnd()){replace(selectionStart(),selectionEnd(),"");return;}if(state.caret)replace(state.caret-1,state.caret,"");}
void CodeEditor::eraseForward(){if(selectionStart()!=selectionEnd()){replace(selectionStart(),selectionEnd(),"");return;}if(state.caret<state.text.size())replace(state.caret,state.caret+1,"");}
void CodeEditor::indent(bool back){
 size_t a=startOfLine(state.text,selectionStart()),b=selectionEnd();
 if(b>a && state.text[b-1]=='\n')--b;
 b=endOfLine(state.text,b);
 std::string part=state.text.substr(a,b-a),result;std::istringstream in(part);std::string row;
 while(std::getline(in,row)){
  if(!result.empty())result+='\n';
  if(back){size_t n=0;while(n<row.size() && n<4 && row[n]==' ')++n;row.erase(0,n);}
  else row="    "+row;
  result+=row;
 }
 if(part.empty() && !back)result="    ";
 if(state.text.size()-part.size()+result.size()>65536)return;
 replace(a,b,result);state.anchor=a;
}
bool CodeEditor::undo(){if(past.empty())return false;future.push_back(state);state=past.back();past.pop_back();return true;}
bool CodeEditor::redo(){if(future.empty())return false;past.push_back(state);state=future.back();future.pop_back();return true;}
size_t CodeEditor::line()const{return static_cast<size_t>(std::count(state.text.begin(),state.text.begin()+state.caret,'\n'));}
std::vector<std::string> CodeEditor::lines()const{
 std::vector<std::string> out;std::string row;for(char c:state.text){if(c=='\n'){out.push_back(row);row.clear();}else row+=c;}out.push_back(row);return out;
}
}
