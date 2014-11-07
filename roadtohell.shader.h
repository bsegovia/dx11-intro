/* File generated with Shader Minifier 1.1.4
 * http://www.ctrl-alt-test.fr
 */
#ifndef ROADTOHELL_SHADER_H_H_TMP_
# define ROADTOHELL_SHADER_H_H_TMP_
# define VAR_PI "f"
# define VAR_OUTPUT "c"

static const char roadtohell_shader_h[] =
 "static const float f=3.14159f;"
 "RWTexture2D<float4> c:register(u0);cbuffer cb:register(b0){float2 _r;float _t;};"
 "float2 n(float2 f,float2 y)"
 "{"
   "return f-y*floor(f/y);"
 "}"
 "float2 s(float2 f,float n)"
 "{"
   "return float2(cos(n)*f.x-sin(n)*f.y,sin(n)*f.x+cos(n)*f.y);"
 "}"
 "float l(float3 c,float y)"
 "{"
   "float m=.125f*cos(f*c.z+y*4.f-f),l=2.f,x=l*.225f,z=length(n(c.xy,l)-l*.5f)-x,s=2.f,t=s*.2125f+s*.0125f*cos(f*c.y*8.f)+s*.0125f*cos(f*c.z*8.f),u=length(n(c.xy,s)-s*.5f)-t,D=1.f,o=D*.5f,a=length(n(c.xz,D).xy-D*.5f)-o,e=.25f,r=e*.375f,i=length(n(c.yz,e).xy-e*.5f)-r,h=.5f,g=h*.25f+.125f*sin(f*c.z*2.f),d=length(n(c.xy,h).xy-h*.5f)-g,b=.075f*sin(f*c.z),T=.5f+b,p=T*.025f+.00125f*cos(f*c.z*128.f),W=length(n(c.xy,T)-T*.5f)-p,V=.05f,S=V*.5f+.025f*cos(f*c.z*2.f),R=length(n(c.yz,V)-V*.5f)-S;"
   "return max(min(min(-z,lerp(u,-R,.375f)),max(min(-a,i),-d)),-W);"
 "}"
 "[numthreads(8,8,1)]"
 "void main(uint3 n:SV_DispatchThreadID)"
 "{"
   "float y=_t*.2975f,x=1.f-.325f*sin(f*y*.25f),z=1.f,e=.5f;"
   "float2 m=float2(n.xy),t=m.xy/_r.xy,h=-1.f+2.f*t;"
   "float3 u=normalize(float3(h*float2(1.77f,1.f),1.f));"
   "u.zx=s(u.zx,-f*y*.25f);"
   "u.xy=s(u.xy,-y*.5f);"
   "float3 i=float3(x,z,e-y*2.5f);"
   "float r=0.f;"
   "const int S=96;"
   "for(int D=0;D<S;D++)"
     "{"
       "float T=l(i+u*r,y);"
       "r+=T*.75f;"
     "}"
   "float3 T=i+u*r;"
   "float2 b=float2(-.0025f,.002f);"
   "float3 o=normalize(float3(l(T+b.xyx,y),l(T+b.yxy,y),l(T+b.yyx,y)));"
   "float V=(o.x+o.y+o.z)*.35f;"
   "float3 a=float3(V,V,V)+r*.0625f;"
   "c[n.xy]=float4(float3(V-r*.0375f+h.y*.05f,V-r*.025f-h.y*.0625f,V+r*.025f-h.y*.025f)+a*a,1.f);"
 "}";

#endif // ROADTOHELL_SHADER_H_H_TMP_
