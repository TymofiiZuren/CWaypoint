#include "code_editor.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <set>
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
namespace {
size_t quotedEnd(const std::string& s,size_t p){
 char quote=s[p++];while(p<s.size()){if(s[p]=='\\'){p=std::min(s.size(),p+2);continue;}if(s[p++]==quote)break;}return p;
}
size_t rawEnd(const std::string& s,size_t p){
 auto open=s.find('(',p+2);if(open==s.npos||open-p>18)return p+1;
 auto close=s.find(")"+s.substr(p+2,open-p-2)+"\"",open+1);if(close==s.npos)return s.size();return close+open-p;
}
bool identifier(char c){return std::isalnum(static_cast<unsigned char>(c))||c=='_';}
SyntaxKind wordKind(const std::string& word){
 static const std::set<std::string> keywords{"alignas","alignof","asm","auto","break","case","catch","class","const","constexpr","consteval","constinit","continue","default","delete","do","else","enum","explicit","export","extern","false","for","friend","goto","if","inline","mutable","namespace","new","noexcept","nullptr","operator","private","protected","public","register","requires","return","sizeof","static","static_assert","struct","switch","template","this","thread_local","throw","true","try","typedef","typeid","typename","union","using","virtual","volatile","while","_Alignas","_Alignof","_Atomic","_Generic","_Noreturn","_Static_assert","_Thread_local"};
 static const std::set<std::string> types{"bool","char","char8_t","char16_t","char32_t","double","float","int","long","short","signed","unsigned","void","wchar_t","_Bool","size_t","FILE","std","string","vector","array","optional","unique_ptr","shared_ptr"};
 if(keywords.count(word))return SyntaxKind::Keyword;if(types.count(word))return SyntaxKind::Type;return SyntaxKind::Plain;
}
size_t numberEnd(const std::string& s,size_t p){
 ++p;while(p<s.size()){
  char c=s[p];if(identifier(c)||c=='.'||c=='\''){++p;continue;}
  if((c=='+'||c=='-')&&std::string("eEpP").find(s[p-1])!=std::string::npos){++p;continue;}break;
 }return p;
}
SyntaxToken wordToken(const std::string& s,size_t p){
 size_t end=p+1;while(end<s.size()&&identifier(s[end]))++end;auto kind=wordKind(s.substr(p,end-p));
 size_t next=s.find_first_not_of(" \t\r\n",end);if(kind==SyntaxKind::Plain&&next<s.size()&&s[next]=='(')kind=SyntaxKind::Function;
 return {p,end-p,kind};
}
std::string indentRow(std::string row,bool back,size_t& removed){
 if(!back)return "    "+row;size_t n=0;while(n<row.size()&&n<4&&row[n]==' ')++n;
 if(!n&&!row.empty()&&row[0]=='\t')n=1;row.erase(0,n);removed=n;return row;
}
SyntaxToken tokenAt(const std::string& s,size_t p){
 size_t end=p+1;SyntaxKind kind=SyntaxKind::Plain;
 if(s.compare(p,2,"//")==0){end=s.find('\n',p);if(end==s.npos)end=s.size();kind=SyntaxKind::Comment;}
 else if(s.compare(p,2,"/*")==0){end=s.find("*/",p+2);end=end==s.npos?s.size():end+2;kind=SyntaxKind::Comment;}
 else if(s.compare(p,2,"R\"")==0){end=rawEnd(s,p);kind=SyntaxKind::String;}
 else if(s[p]=='\"'||s[p]=='\''){end=quotedEnd(s,p);kind=SyntaxKind::String;}
 else if(s[p]=='#'){end=s.find('\n',p);if(end==s.npos)end=s.size();kind=SyntaxKind::Directive;}
 else if(std::isdigit(static_cast<unsigned char>(s[p]))){end=numberEnd(s,p);kind=SyntaxKind::Number;}
 else if(identifier(s[p]))return wordToken(s,p);
 return {p,end-p,kind};
}
std::string leadingIndent(const std::string& s,size_t start){size_t end=start;while(end<s.size()&&(s[end]==' '||s[end]=='\t'))++end;return s.substr(start,end-start);}
std::string codeOnly(const std::string& s){
 std::string result=s;for(const auto& t:lexCxx(s))if(t.kind==SyntaxKind::String||t.kind==SyntaxKind::Comment||t.kind==SyntaxKind::Directive)result.replace(t.start,t.length,t.length,' ');return result;
}
bool openString(const std::string& source,const SyntaxToken& token,size_t end){
 if(source.compare(token.start,2,"R\"")==0){
  auto open=source.find('(',token.start+2);if(open>=end)return true;
  auto marker=")"+source.substr(token.start+2,open-token.start-2)+"\"";
  return end<marker.size()||source.compare(end-marker.size(),marker.size(),marker)!=0;
 }
 char quote=source[token.start];for(size_t i=token.start+1;i<end;++i){if(source[i]=='\\'){++i;continue;}if(source[i]==quote)return false;}return true;
}
bool insideLiteral(const std::string& source,size_t end){
 auto tokens=lexCxx(source.substr(0,end));if(tokens.empty())return false;auto token=tokens.back();
 if(token.start+token.length!=end)return false;
 if(token.kind==SyntaxKind::String)return openString(source,token,end);
 if(token.kind!=SyntaxKind::Comment)return false;
 return source.compare(token.start,2,"//")==0||source.compare(end>=2?end-2:0,2,"*/")!=0;
}
std::string closingIndent(const std::string& s,size_t end){
 auto source=codeOnly(s.substr(0,end));int depth=0;
 for(size_t i=source.size();i>0;){char c=source[--i];if(c=='}')++depth;if(c=='{'){if(depth==0)return leadingIndent(s,startOfLine(s,i));--depth;}}
 auto indent=leadingIndent(s,startOfLine(s,end));indent.resize(indent.size()>4?indent.size()-4:0);return indent;
}
}
std::vector<SyntaxToken> lexCxx(const std::string& source){
 std::vector<SyntaxToken> tokens;size_t p=0;
 while(p<source.size()){
  if(std::isspace(static_cast<unsigned char>(source[p]))){++p;continue;}
  auto token=tokenAt(source,p);tokens.push_back(token);p+=token.length;
 }return tokens;
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
 if(selected().empty()&&!back){auto column=state.caret-startOfLine(state.text,state.caret);insert(std::string(4-column%4,' '));return;}
 size_t a=startOfLine(state.text,selectionStart()),b=selectionEnd();if(b>a&&state.text[b-1]=='\n')--b;b=endOfLine(state.text,b);
 std::string part=state.text.substr(a,b-a),result;size_t p=0,removed=0;bool selection=!selected().empty();auto oldCaret=state.caret;
 do{auto end=part.find('\n',p);if(end==part.npos)end=part.size();auto row=part.substr(p,end-p);
  row=indentRow(row,back,removed);
  result+=row;if(end==part.size())break;result+='\n';p=end+1;
 }while(p<=part.size());
 if(result==part)return;
 if(state.text.size()-part.size()+result.size()>65536)return;
 replace(a,b,result);
 if(selection){state.anchor=a;return;}
 state.caret=state.anchor=std::max(a,oldCaret-std::min(removed,oldCaret-a));
}
void CodeEditor::newline(){
 size_t caret=selectionStart(),start=startOfLine(state.text,caret);auto indent=leadingIndent(state.text,start);indent.resize(std::min(indent.size(),caret-start));
 auto before=codeOnly(state.text.substr(0,caret)).substr(start);auto last=before.find_last_not_of(" \t");bool opens=last!=before.npos&&before[last]=='{';
 auto suffix=state.text.find_first_not_of(" \t",selectionEnd());bool closes=opens&&suffix<state.text.size()&&state.text[suffix]=='}';
 std::string addition="\n"+indent+(opens?"    ":"");size_t position=selectionStart()+addition.size();
 if(closes)addition+="\n"+indent;
 if(closes){
  if(state.text.size()-(suffix-selectionStart())+addition.size()>65536)return;
  replace(selectionStart(),suffix,addition);moveTo(position);
 }else if(insert(addition))moveTo(position);
}
bool CodeEditor::typeCharacter(char c){
 auto start=startOfLine(state.text,state.caret);auto prefix=state.text.substr(start,state.caret-start);
 if(c!='}'||!selected().empty()||prefix.find_first_not_of(" \t")!=prefix.npos)return insert(std::string(1,c));
 if(insideLiteral(state.text,state.caret))return insert("}");
 auto addition=closingIndent(state.text,state.caret)+"}";if(state.text.size()-prefix.size()+addition.size()>65536)return false;
 replace(start,state.caret,addition);return true;
}
bool CodeEditor::undo(){if(past.empty())return false;future.push_back(state);state=past.back();past.pop_back();return true;}
bool CodeEditor::redo(){if(future.empty())return false;past.push_back(state);state=future.back();future.pop_back();return true;}
size_t CodeEditor::line()const{return static_cast<size_t>(std::count(state.text.begin(),state.text.begin()+state.caret,'\n'));}
std::vector<std::string> CodeEditor::lines()const{
 std::vector<std::string> out;std::string row;for(char c:state.text){if(c=='\n'){out.push_back(row);row.clear();}else row+=c;}out.push_back(row);return out;
}
}
