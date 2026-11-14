#pragma once
namespace cw {
struct ReferenceLayout{bool compact;float rail,materialX,materialW,codeX,codeW,bodyTop,bodyBottom;};
inline ReferenceLayout referenceLayout(int width,int height){
 bool compact=width<1280;float rail=compact?0.f:216.f;
 float material=compact ? static_cast<float>(width) : (width-rail)*.4f;
 return {compact,rail,rail,material,rail+material,compact?static_cast<float>(width):width-rail-material,80.f,static_cast<float>(height)-24.f};
}
}
