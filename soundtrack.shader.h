/* File generated with Shader Minifier 1.1.4
 * http://www.ctrl-alt-test.fr
 */
#ifndef SOUNDTRACK_SHADER_H_H_TMP_
# define SOUNDTRACK_SHADER_H_H_TMP_
# define VAR_PI "f"
# define VAR_TAU "t"
# define VAR_G_SOUNDTRACK "i"

static const char soundtrack_shader_h[] =
 "static const float f=3.14159f,t=2.f*f;"
 "RWStructuredBuffer<int> i:register(u0);"
 "float2 r(float f)"
 "{"
   "return float2(0.f,0.f);"
 "}"
 "[numthreads(1024,1,1)]"
 "void main(uint2 f:SV_DispatchThreadID)"
 "{"
   "float2 t=r(float(f.x+f.y*65536)/float(44100));"
   "int2 c=int2(32767.f*t)&65535;"
   "i[f.x+f.y*65536]=c.x|c.y<<16;"
 "}";

#endif // SOUNDTRACK_SHADER_H_H_TMP_
