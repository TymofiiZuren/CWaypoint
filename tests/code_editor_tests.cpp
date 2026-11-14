#include "code_editor.hpp"
#include <stdexcept>
#include <iostream>
using namespace cw;
void check(bool b){if(!b)throw std::runtime_error("code editor contract");}
int main(){
 CodeEditor e;check(e.set("a\nb\n"));e.moveTo(2);e.insert("four");check(e.value()=="a\nfourb\n");
 e.undo();check(e.value()=="a\nb\n");e.redo();check(e.value()=="a\nfourb\n");
 e.set("one\ntwo");e.moveTo(1);e.vertical(1);check(e.cursor()==5);e.home();check(e.cursor()==4);
 e.selectAll();e.indent(false);check(e.value()=="    one\n    two");e.indent(true);check(e.value()=="one\ntwo");
 e.set("abc");e.moveTo(1);e.moveTo(2,true);e.insert("X\nY");check(e.value()=="aX\nYc");
 check(!e.set(std::string(65537,'x')));check(!e.insert(std::string(1,'\1')));
 e.set("abc");e.moveTo(3);e.backspace();check(e.value()=="ab");e.undo();check(e.value()=="abc");
 std::cout<<"Multiline editing contracts passed\n";
}
