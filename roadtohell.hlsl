static const float PI=3.14159265358979323846f;

#define RESOLUTION _r
#define GLOBALTIME _t
RWTexture2D<float4> output : register (u0);
cbuffer cb : register(b0) {
  float2 RESOLUTION;
  float GLOBALTIME;
};

float2 mod(float2 x, float2 y) { return x - y*floor(x/y); }
float2 rotate(float2 k,float t) {
  return float2(cos(t)*k.x-sin(t)*k.y,sin(t)*k.x+cos(t)*k.y);
}

float draw_scene(float3 p, float speed) {
  float tunnel_m=0.125f*cos(PI*p.z*1.0f+speed*4.0f-PI);
  float tunnel1_p=2.0f;
  float tunnel1_w=tunnel1_p*0.225f;
  float tunnel1=length(mod(p.xy,tunnel1_p)-tunnel1_p*0.5f)-tunnel1_w;  // tunnel1
  float tunnel2_p=2.0f;
  float tunnel2_w=tunnel2_p*0.2125f+tunnel2_p*0.0125f*cos(PI*p.y*8.0f)+tunnel2_p*0.0125f*cos(PI*p.z*8.0f);
  float tunnel2=length(mod(p.xy,tunnel2_p)-tunnel2_p*0.5f)-tunnel2_w;  // tunnel2
  float hole1_p=1.0f;
  float hole1_w=hole1_p*0.5f;
  float hole1=length(mod(p.xz,hole1_p).xy-hole1_p*0.5f)-hole1_w;  // hole1
  float hole2_p=0.25f;
  float hole2_w=hole2_p*0.375f;
  float hole2=length(mod(p.yz,hole2_p).xy-hole2_p*0.5f)-hole2_w;  // hole2
  float hole3_p=0.5f;
  float hole3_w=hole3_p*0.25f+0.125f*sin(PI*p.z*2.0f);
  float hole3=length(mod(p.xy,hole3_p).xy-hole3_p*0.5f)-hole3_w;  // hole3
  float tube_m=0.075f*sin(PI*p.z*1.0f);
  float tube_p=0.5f+tube_m;
  float tube_w=tube_p*0.025f+0.00125f*cos(PI*p.z*128.0f);
  float tube=length(mod(p.xy,tube_p)-tube_p*0.5f)-tube_w;    // tube
  float bubble_p=0.05f;
  float bubble_w=bubble_p*0.5f+0.025f*cos(PI*p.z*2.0f);
  float bubble=length(mod(p.yz,bubble_p)-bubble_p*0.5f)-bubble_w;  // bubble
  return max(min(min(-tunnel1,lerp(tunnel2,-bubble,0.375f)),max(min(-hole1,hole2),-hole3)),-tube);
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
  float speed=GLOBALTIME*0.2975f;
  float ground_x=1.0f-0.325f*sin(PI*speed*0.25f);
  float ground_y=1.0f;
  float ground_z=0.5f;
  float2 gl_FragCoord = float2(DTid.xy);
  float2 position=(gl_FragCoord.xy/RESOLUTION.xy);
  float2 p=-1.0f+2.0f*position;
  float3 dir=normalize(float3(p*float2(1.77f,1.0f),1.0f));  // screen ratio (x,y) fov (z)
  dir.zx=rotate(dir.zx,-PI*speed*0.25f);            // rotation y
  dir.xy=rotate(dir.xy,-speed*0.5f);              // rotation z
  float3 ray=float3(ground_x,ground_y,ground_z-speed*2.5f);
  float t=0.0f;
  const int ray_n=96;
  for(int i=0;i<ray_n;i++) {
  float k=draw_scene(ray+dir*t, speed);
  t+=k*0.75f;
  }
  float3 hit=ray+dir*t;
  float2 h=float2(-0.0025f,0.002f); // light
  float3 n=normalize(float3(draw_scene(hit+h.xyx, speed),draw_scene(hit+h.yxy, speed),draw_scene(hit+h.yyx, speed)));
  float c=(n.x+n.y+n.z)*0.35f;
  float3 color=float3(c,c,c)+t*0.0625f;
  output[DTid.xy] = float4(float3(c-t*0.0375f+p.y*0.05f,c-t*0.025f-p.y*0.0625f,c+t*0.025f-p.y*0.025f)+color*color,1.0f);
}

