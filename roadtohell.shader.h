/* File generated with Shader Minifier 1.1.4
 * http://www.ctrl-alt-test.fr
 */
#ifndef ROADTOHELL_SHADER_H_H_TMP_
# define ROADTOHELL_SHADER_H_H_TMP_
# define VAR_PI "f"
# define VAR_OUTPUT "c"

static const char roadtohell_shader_h[] =
 "static const float f=3.14159f;"
 "RWTexture2D<float4> c:register(u0);cbuffer cb:register(b0){float2 iResolution;float iGlobalTime;};"
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
   "float i=.125f*cos(f*c.z+y*4.f-f),l=2.f,x=l*.225f,z=length(n(c.xy,l)-l*.5f)-x,s=2.f,m=s*.2125f+s*.0125f*cos(f*c.y*8.f)+s*.0125f*cos(f*c.z*8.f),u=length(n(c.xy,s)-s*.5f)-m,e=1.f,t=e*.5f,o=length(n(c.xz,e).xy-e*.5f)-t,D=.25f,a=D*.375f,r=length(n(c.yz,D).xy-D*.5f)-a,h=.5f,g=h*.25f+.125f*sin(f*c.z*2.f),b=length(n(c.xy,h).xy-h*.5f)-g,T=.075f*sin(f*c.z),R=.5f+T,d=R*.025f+.00125f*cos(f*c.z*128.f),G=length(n(c.xy,R)-R*.5f)-d,p=.05f,W=p*.5f+.025f*cos(f*c.z*2.f),V=length(n(c.yz,p)-p*.5f)-W;"
   "return max(min(min(-z,lerp(u,-V,.375f)),max(min(-o,r),-b)),-G);"
 "}"
 "[numthreads(8,8,1)]"
 "void main(uint3 n:SV_DispatchThreadID)"
 "{"
   "float y=iGlobalTime*.2975f,x=1.f-.325f*sin(f*y*.25f),z=1.f,e=.5f;"
   "float2 i=float2(n.xy),r=i.xy/iResolution.xy,m=-1.f+2.f*r;"
   "float3 u=normalize(float3(m*float2(1.77f,1.f),1.f));"
   "u.zx=s(u.zx,-f*y*.25f);"
   "u.xy=s(u.xy,-y*.5f);"
   "float3 h=float3(x,z,e-y*2.5f);"
   "float R=0.f;"
   "const int W=96;"
   "for(int D=0;D<W;D++)"
     "{"
       "float p=l(h+u*R,y);"
       "R+=p*.75f;"
     "}"
   "float3 p=h+u*R;"
   "float2 T=float2(-.0025f,.002f);"
   "float3 b=normalize(float3(l(p+T.xyx,y),l(p+T.yxy,y),l(p+T.yyx,y)));"
   "float t=(b.x+b.y+b.z)*.35f;"
   "float3 o=float3(t,t,t)+R*.0625f;"
   "c[n.xy]=float4(float3(t-R*.0375f+m.y*.05f,t-R*.025f-m.y*.0625f,t+R*.025f-m.y*.025f)+o*o,1.f);"
 "}";

#endif // ROADTOHELL_SHADER_H_H_TMP_
