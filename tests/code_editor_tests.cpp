#include "code_editor.hpp"
#include <stdexcept>
#include <iostream>
using namespace cw;
void require(bool b,int line){if(!b)throw std::runtime_error("code editor contract at line "+std::to_string(line));}
#define check(value) require((value),__LINE__)
int main(){
 CodeEditor e;check(e.set("a\nb\n"));e.moveTo(2);e.insert("four");check(e.value()=="a\nfourb\n");
 e.undo();check(e.value()=="a\nb\n");e.redo();check(e.value()=="a\nfourb\n");
 e.set("one\ntwo");e.moveTo(1);e.vertical(1);check(e.cursor()==5);e.home();check(e.cursor()==4);
 e.selectAll();e.indent(false);check(e.value()=="    one\n    two");e.indent(true);check(e.value()=="one\ntwo");
 e.set("abc");e.moveTo(1);e.moveTo(2,true);e.insert("X\nY");check(e.value()=="aX\nYc");
 check(!e.set(std::string(65537,'x')));check(!e.insert(std::string(1,'\1')));
 e.set("abc");e.moveTo(3);e.backspace();check(e.value()=="ab");e.undo();check(e.value()=="abc");
 e.set("abc");e.moveTo(3);e.indent(false);check(e.value()=="abc " && e.selected().empty() && e.cursor()==4);e.insert("x");check(e.value()=="abc x");
 e.set("    value;");e.moveTo(8);e.indent(true);check(e.value()=="value;"&&e.cursor()==4&&e.selected().empty());e.undo();check(e.value()=="    value;"&&e.cursor()==8);
 e.set("one\n\ntwo\nlast");e.moveTo(0);e.moveTo(9,true);e.indent(false);check(e.value()=="    one\n    \n    two\nlast");e.indent(true);check(e.value()=="one\n\ntwo\nlast");
 e.set("if (ready) {");e.moveTo(e.value().size());e.newline();check(e.value()=="if (ready) {\n    "&&e.selected().empty());e.typeCharacter('}');check(e.value()=="if (ready) {\n}");e.undo();check(e.value()=="if (ready) {\n    ");
 e.set("    if (ready) {}");e.moveTo(16);e.newline();check(e.value()=="    if (ready) {\n        \n    }"&&e.cursor()==25);e.undo();check(e.value()=="    if (ready) {}");
 e.set("    printf(\"{\");");e.moveTo(e.value().size());e.newline();check(e.value()=="    printf(\"{\");\n    ");
 e.set("if (ready) { // comment");e.moveTo(e.value().size());e.newline();check(e.value()=="if (ready) { // comment\n    ");
 auto kinds=lexCxx("#include <stdio.h>\nint main(){ // int 20\n const char* s=\"return \\\"42\"; return 2+3; /* block\nif */ }");
 bool keyword=false,type=false,string=false,number=false,comment=false,directive=false,function=false;
 for(auto& t:kinds){keyword|=t.kind==SyntaxKind::Keyword;type|=t.kind==SyntaxKind::Type;string|=t.kind==SyntaxKind::String;number|=t.kind==SyntaxKind::Number;comment|=t.kind==SyntaxKind::Comment;directive|=t.kind==SyntaxKind::Directive;function|=t.kind==SyntaxKind::Function;}
 check(keyword&&type&&string&&number&&comment&&directive&&function);
 std::string literal="R\"tag({ // text })tag\"";auto raw=lexCxx(literal);check(raw.size()==1&&raw[0].length==literal.size()&&raw[0].kind==SyntaxKind::String);
 e.set("if (ok) {\n    /* comment\n        ");e.moveTo(e.value().size());e.typeCharacter('}');check(e.value()=="if (ok) {\n    /* comment\n        }");
 e.set("x");e.moveTo(1);e.indent(false);e.indent(true);check(e.value()=="x   "&&e.selected().empty());
 auto numbers=lexCxx("1e-3 + 2+3");check(numbers.size()==5&&numbers[0].length==4&&numbers[3].kind==SyntaxKind::Plain);
 e.set("if (x) {    }");e.moveTo(e.value().find('{')+1);e.newline();check(e.value()=="if (x) {\n    \n}");
 e.set("x");e.moveTo(1);e.indent(false);e.indent(true);e.undo();check(e.value()=="x");
 e.set("void f() {\n    auto s = R\"(\n        ");e.moveTo(e.value().size());e.typeCharacter('}');check(e.value()=="void f() {\n    auto s = R\"(\n        }");
 std::cout<<"Multiline editing contracts passed\n";
}
