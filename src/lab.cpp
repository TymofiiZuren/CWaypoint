#include "lab.hpp"
#include <CommonCrypto/CommonDigest.h>
#include <iomanip>
#include <sstream>
namespace cw {
std::string sourceHash(const std::string& source){
 unsigned char digest[CC_SHA256_DIGEST_LENGTH];CC_SHA256(source.data(),static_cast<CC_LONG>(source.size()),digest);
 std::ostringstream out;for(unsigned char c:digest)out<<std::hex<<std::setw(2)<<std::setfill('0')<<static_cast<int>(c);return out.str();
}
LabExercise labFor(const std::string& id){
 LabExercise e;e.lessonId=id;e.language=id=="cpp-strings" ? "cpp":"c";
 if(id=="cpp-strings"){
  e.instructions="Read a name, then print Hello, followed by the name and an exclamation mark. Run both examples.";
  e.solution="#include <iostream>\n#include <string>\n\nint main() {\n    std::string name;\n    std::cin >> name;\n    std::cout << \"Hello, \" << name << \"!\\n\";\n    return 0;\n}\n";
  e.starter="#include <iostream>\n#include <string>\n\nint main() {\n    std::string name;\n    std::cin >> name;\n    // Include name in the greeting.\n    std::cout << \"Hello!\\n\";\n    return 0;\n}\n";
  e.cases={{"Ada\n","Hello, Ada!\n"},{"Lin\n","Hello, Lin!\n"}};return e;
 }
 if(id=="c-conditions"){
  e.instructions="Read an age. Print Adult when age is at least 18; otherwise print Minor. Check both branches.";
  e.solution="#include <stdio.h>\n\nint main(void) {\n    int age = 0;\n    scanf(\"%d\", &age);\n    if (age >= 18) {\n        printf(\"Adult\\n\");\n    } else {\n        printf(\"Minor\\n\");\n    }\n    return 0;\n}\n";
  e.starter="#include <stdio.h>\n\nint main(void) {\n    int age = 0;\n    scanf(\"%d\", &age);\n    // Add the age condition and both outputs.\n    printf(\"Minor\\n\");\n    return 0;\n}\n";
  e.cases={{"20\n","Adult\n"},{"12\n","Minor\n"}};return e;
 }
 if(id=="c-pointers"){
  e.instructions="Read an integer into value, use a pointer to increase it by one, then print the value.";
  e.solution="#include <stdio.h>\n\nint main(void) {\n    int value = 0;\n    scanf(\"%d\", &value);\n    int *pointer = &value;\n    *pointer = *pointer + 1;\n    printf(\"%d\\n\", value);\n    return 0;\n}\n";
  e.starter="#include <stdio.h>\n\nint main(void) {\n    int value = 0;\n    scanf(\"%d\", &value);\n    int *pointer = &value;\n    // Increase the pointed-to value.\n    printf(\"%d\\n\", value);\n    return 0;\n}\n";
  e.cases={{"4\n","5\n"},{"-1\n","0\n"}};return e;
 }
 e.instructions="Read two integers and print their sum. The two tests include a negative value.";
 e.solution="#include <stdio.h>\n\nint main(void) {\n    int a = 0, b = 0;\n    scanf(\"%d %d\", &a, &b);\n    printf(\"%d\\n\", a + b);\n    return 0;\n}\n";
 e.starter="#include <stdio.h>\n\nint main(void) {\n    int a = 0, b = 0;\n    scanf(\"%d %d\", &a, &b);\n    // Print the sum of a and b.\n    printf(\"%d\\n\", a);\n    return 0;\n}\n";
 e.cases={{"2 3\n","5\n"},{"-1 4\n","3\n"}};return e;
}
namespace {
std::string request(const LabExercise& e,const std::string& source){
 std::ostringstream out;out<<"CWLAB1 "<<e.language<<' '<<std::quoted(source)<<' '<<e.cases.size()<<'\n';
 for(const auto& test:e.cases)out<<std::quoted(test.input)<<'\n';return out.str();
}
bool validResponse(const std::string& magic,int compiled,size_t count,size_t expected){
 return magic=="CWRESULT1" && count<=expected && (!compiled || count==expected);
}
LabResult readResult(const LabExercise& e,const ProcessResult& p,const std::string& hash){
 LabResult r;r.sourceHash=hash;r.testRevision=e.revision;r.cancelled=p.cancelled;
 if(p.exitCode!=0){r.diagnostics=p.error.empty()?"Runner stopped or is unavailable.":p.error;return r;}
 std::istringstream in(p.output);std::string magic;int compiled;size_t count;
 if(!(in>>magic>>compiled>>std::quoted(r.diagnostics)>>count) || !validResponse(magic,compiled,count,e.cases.size())){r.diagnostics="Invalid isolated runner response.";return r;}
 r.compiled=compiled;
 for(size_t i=0;i<count;++i){
  ProcessResult run;
  if(!(in>>run.exitCode>>run.timedOut>>run.cancelled>>run.outputLimited>>std::quoted(run.output)>>std::quoted(run.error))){r.compiled=false;r.diagnostics="Incomplete runner response.";return r;}
  if(run.exitCode==0 && !run.timedOut && !run.outputLimited && !run.cancelled && run.output==e.cases[i].expected)++r.passed;
  r.runs.push_back(std::move(run));
 }
 return r;
}
}
LabResult runLab(const LabExercise& e,const std::string& source,const std::filesystem::path& helper,
 const std::filesystem::path& tools,const std::atomic_bool* cancel){
 LabResult r;r.sourceHash=sourceHash(source);r.testRevision=e.revision;
 if(source.empty() || source.size()>65536 || e.cases.empty() || e.cases.size()>16){r.diagnostics="Enter a complete program (maximum 64 KiB).";return r;}
 auto p=runProcess({helper.string(),tools.string()},request(e,source),55,cancel,2200000);
 return readResult(e,p,r.sourceHash);
}
}
