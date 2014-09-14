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
 "float2 n(float2 f,float2 i)"
 "{"
   "return f-floor(f/i);"
 "}"
 "float2 s(float2 f,float n)"
 "{"
   "return float2(cos(n)*f.x-sin(n)*f.y,sin(n)*f.x+cos(n)*f.y);"
 "}"
 "float l(float3 c,float i)"
 "{"
   "float m=.125f*cos(f*c.z+i*4.f-f),l=2.f,y=l*.225f,x=length(n(c.xy,l)-l*.5f)-y,z=2.f,u=z*.2125f+z*.0125f*cos(f*c.y*8.f)+z*.0125f*cos(f*c.z*8.f),D=length(n(c.xy,z)-z*.5f)-u,s=1.f,t=s*.5f,o=length(n(c.xz,s).xy-s*.5f)-t,e=.25f,a=e*.375f,r=length(n(c.yz,e).xy-e*.5f)-a,h=.5f,g=h*.25f+.125f*sin(f*c.z*2.f),T=length(n(c.xy,h).xy-h*.5f)-g,G=.075f*sin(f*c.z),p=.5f+G,d=p*.025f+.00125f*cos(f*c.z*128.f),b=length(n(c.xy,p)-p*.5f)-d,V=.05f,S=V*.5f+.025f*cos(f*c.z*2.f),I=length(n(c.yz,V)-V*.5f)-S;"
   "return max(min(min(-x,lerp(D,-I,.375f)),max(min(-o,r),-T)),-b);"
 "}"
 "[numthreads(8,8,1)]"
 "void main(uint3 i:SV_GroupID,uint3 n:SV_DispatchThreadID,uint3 m:SV_GroupThreadID,uint b:SV_GroupIndex)"
 "{"
   "float z=iGlobalTime*.2975f,y=1.f-.325f*sin(f*z*.25f),x=1.f,u=.5f;"
   "float2 e=float2(n.xy),r=e.xy/iResolution.xy,h=-1.f+2.f*r;"
   "float3 p=normalize(float3(h*float2(1.77f,1.f),1.f));"
   "p.zx=s(p.zx,-f*z*.25f);"
   "p.xy=s(p.xy,-z*.5f);"
   "float3 D=float3(y,x,u-z*2.5f);"
   "float V=0.f;"
   "const int S=96;"
   "for(int t=0;t<S;t++)"
     "{"
       "float o=l(D+p*V,z);"
       "V+=o*.75f;"
     "}"
   "float3 o=D+p*V;"
   "float2 G=float2(-.0025f,.002f);"
   "float3 a=normalize(float3(l(o+G.xyx,z),l(o+G.yxy,z),l(o+G.yyx,z)));"
   "float g=(a.x+a.y+a.z)*.35f;"
   "float3 d=float3(g,g,g)+V*.0625f;"
   "c[n.xy]=float4(float3(g-V*.0375f+h.y*.05f,g-V*.025f-h.y*.0625f,g+V*.025f-h.y*.025f)+d*d,1.f);"
 "}";

#endif // ROADTOHELL_SHADER_H_H_TMP_
