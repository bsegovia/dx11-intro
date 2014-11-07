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
 "float c(float f,float y)"
 "{"
   "return exp2(-f*(5./y));"
 "}"
 "float c(float i,float f,float y)"
 "{"
   "if(i<f)"
     "return i/f;"
   "i-=f;"
   "return c(i,y);"
 "}"
 "float c(float f)"
 "{"
   "return 440.*pow(2.,(f-69.)/12.);"
 "}"
 "float s(float f)"
 "{"
   "return abs(frac(f)*4.-2.)-1.;"
 "}"
 "float r(float f)"
 "{"
   "return frac(f)*2.-1.;"
 "}"
 "float d(float f)"
 "{"
   "return cos(f*radians(360.));"
 "}"
 "float p(float f)"
 "{"
   "return step(frac(f),.5)*2.-1.;"
 "}"
 "float4 a(float4 f)"
 "{"
   "f=abs(frac(f)*2.-1.);"
   "float4 y=f*f;"
   "return y*6.-y*f*4.-1.;"
 "}"
 "float4 l(float4 f)"
 "{"
   "return frac(f)*2.-1.;"
 "}"
 "float e(float f)"
 "{"
   "float2 i=frac(float2(f*5.3983,f*5.4427));"
   "i+=dot(i.yx,i.xy+float2(21.5351,14.3137));"
   "return frac(i.x*i.y*95.4337);"
 "}"
 "float n(float f)"
 "{"
   "return e(floor(f*32.))*2.-1.;"
 "}"
 "float m(float f)"
 "{"
   "float y=f*32.,i=floor(y),m=frac(y),u=e(i),t=e(i+1.),x=m*m*(3.-2.*m);"
   "return lerp(u,t,x)*2.-1.;"
 "}"
 "float a(float f,float y)"
 "{"
   "float i=0.,r=1.,t=0.;"
   "i+=m(f)*r;"
   "t+=r;"
   "f*=2.02;"
   "r*=y;"
   "i+=m(f)*r;"
   "t+=r;"
   "f*=2.02;"
   "r*=y;"
   "i+=m(f)*r;"
   "t+=r;"
   "f*=2.02;"
   "r*=y;"
   "i+=m(f)*r;"
   "t+=r;"
   "f*=2.02;"
   "r*=y;"
   "t+=r;"
   "return i/t;"
 "}"
 "float d(float f,float y)"
 "{"
   "float i=f*y,t=floor(i),r=e(t);"
   "return r*2.-1.;"
 "}"
 "float a(float f,float4 y,float4 i,float4 t)"
 "{"
   "return dot(a((f+y)*i),t);"
 "}"
 "float c(float f,float4 y,float4 i,float4 t)"
 "{"
   "return dot(c((f+y)*i),t);"
 "}"
 "float2 u(float f)"
 "{"
   "float2 y=float2(-1.f,-1.f);"
   "float i=0.f;"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(28);"
   "if(f>i)"
     "y=float2(52,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(50,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(48,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(46,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(28);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(47,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(48,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(52,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(50,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(28);"
   "if(f>i)"
     "y=float2(48,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(46,i);"
   "i+=float(126);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(52,i);"
   "i+=float(28);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(50,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(48,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(46,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(28);"
   "if(f>i)"
     "y=float2(47,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(48,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(52,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(50,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(63,i);"
   "i+=float(14);"
   "if(f>i)"
     "y=float2(60,i);"
   "i+=float(14);"
   "if(f>i)"
     "y=float2(59,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(63,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(66,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(64,i);"
   "i+=float(14);"
   "if(f>i)"
     "y=float2(63,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(59,i);"
   "i+=float(14);"
   "if(f>i)"
     "y=float2(63,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(64,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(66,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(67,i);"
   "i+=float(14);"
   "if(f>i)"
     "y=float2(66,i);"
   "i+=float(14);"
   "if(f>i)"
     "y=float2(64,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(63,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(59,i);"
   "i+=float(12);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(26);"
   "if(f>i)"
     "y=float2(52,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(28);"
   "if(f>i)"
     "y=float2(40,i);"
   "i+=float(24);"
   "if(f>i)"
     "y=float2(50,i);"
   "return y;"
 "}"
 "float2 e(float2 f,float i)"
 "{"
   "return float2(c(f.x),(i-f.y)*.005f);"
 "}"
 "float w(float f)"
 "{"
   "return c(f,float4(0.,.5,.1,.4),float4(1.,1.5,2.,-3.),float4(1.,.5,.25,.125));"
 "}"
 "float o(float f)"
 "{"
   "return w(f);"
 "}"
 "float y(float2 f)"
 "{"
   "return o(f.x*f.y)*c(f.y,.01,1.);"
 "}"
 "float x(float2 f)"
 "{"
   "return y(f*float2(2.,1.))*.75;"
 "}"
 "float h(float f)"
 "{"
   "if(f<0.)"
     "return 0.;"
   "float i=f*200.f,y=0.;"
   "y+=x(e(u(i),i));"
   "return y;"
 "}"
 "float2 S(float f)"
 "{"
   "float i=h(f);"
   "return float2(i,i);"
 "}"
 "[numthreads(1024,1,1)]"
 "void main(uint2 f:SV_DispatchThreadID)"
 "{"
   "float2 y=S(float(f.x+f.y*65536)/float(44100));"
   "int2 r=int2(32767.f*y)&65535;"
   "i[f.x+f.y*65536]=r.x|r.y<<16;"
 "}";

#endif // SOUNDTRACK_SHADER_H_H_TMP_
