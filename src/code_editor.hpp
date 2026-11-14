#pragma once
#include <deque>
#include <string>
#include <vector>
namespace cw {
enum class SyntaxKind {Plain,Keyword,Type,Number,String,Comment,Directive,Function};
struct SyntaxToken{size_t start,length;SyntaxKind kind;};
std::vector<SyntaxToken> lexCxx(const std::string& source);
class CodeEditor {
 struct State{std::string text;size_t caret=0,anchor=0;};
 State state;std::deque<State> past,future;
 void remember();void replace(size_t,size_t,const std::string&);
 public:
 const std::string& value()const{return state.text;}
 size_t cursor()const{return state.caret;}
 size_t selectionStart()const;size_t selectionEnd()const;
 std::string selected()const;bool set(const std::string&);bool insert(const std::string&);
 void moveTo(size_t,bool extend=false);void move(int,bool extend=false,bool word=false);
 void vertical(int,bool extend=false);void home(bool extend=false);void end(bool extend=false);
 void selectAll();void backspace();void eraseForward();void indent(bool back);void newline();bool typeCharacter(char);
 bool undo();bool redo();size_t line()const;std::vector<std::string> lines()const;
};
}
