#include "reference_layout.hpp"
#include <stdexcept>
#include <initializer_list>
int main(){for(int width:{960,1280,1600,2036})for(int height:{640,840}){
 auto l=cw::referenceLayout(width,height);if(l.bodyBottom<=l.bodyTop || l.materialW<400)throw std::runtime_error("material bounds");
 if(!l.compact && (l.codeW<600 || l.rail+l.materialW+l.codeW!=width))throw std::runtime_error("workspace bounds");
}}
