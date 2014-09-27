static const float PI=3.14159265358979323846f;

#define RESOLUTION _r
#define GLOBALTIME _t
RWTexture2D<float4> output : register (u0);
cbuffer cb : register(b0) {
  float2 RESOLUTION;
  float GLOBALTIME;
};

#if 0
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
#else
#if 1
float3 hsv2rgb(float3 c) {
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

struct Material {
  float3 rho_d;
  float3 rho_s;
  float3 alpha;
  float3 pm;
  float3 F_0;
  float3 F_1;
  float3 K_ap;
  float3 sh_lambda;
  float3 sh_c;
  float3 sh_k;
  float3 sh_theta0;
};

Material Aluminium() {
  Material m;
  m.rho_d = float3(0.0305166f, 0.0358788f, 0.0363463f);
  m.rho_s = float3(0.0999739f, 0.131797f, 0.0830361f);
  m.alpha = float3(0.0012241f, 0.000926487f, 0.000991844f);
  m.pm = float3(0.537669f, 0.474562f, 0.435936f);
  m.F_0 = float3(0.977854f, 0.503108f, 1.77905f);
  m.F_1 = float3(-0.0221457f, -0.0995445f, 0.77905f);
  m.K_ap = float3(449.321f, 658.044f, 653.86f);
  m.sh_lambda = float3(8.2832e-07f, 9.94692e-08f, 6.11887e-08f);
  m.sh_c = float3(3.54592e-07f, 16.0175f, 15.88f);
  m.sh_k = float3(23.8656f, 10.6911f, 9.69801f);
  m.sh_theta0 = float3(-0.510356f, 0.570179f, 0.566156f);
  return m;
}

Material BlackFabric() {
  Material m;
  m.rho_d = float3(0.0189017, 0.0112353, 0.0110067);
  m.rho_s = float3(2.20654e-16, 6.76197e-15, 1.57011e-13);
  m.alpha = float3(0.132262, 0.128044, 0.127838);
  m.pm = float3(0.189024, 0.18842, 0.188426);
  m.F_0 = float3(1, 1, 1);
  m.F_1 = float3(0, 0, 0);
  m.K_ap = float3(8.1593, 8.38075, 8.39184);
  m.sh_lambda = float3(3.83017, 3.89536, 3.89874);
  m.sh_c = float3(0.00415117, 0.00368324, 0.00365826);
  m.sh_k = float3(12.9974, 13.2597, 13.2737);
  m.sh_theta0 = float3(0.207997, 0.205597, 0.205424);
  return m;
}

Material BlackSoftPlastic() {
  Material m;
  m.rho_d = float3(0.00820133, 0.00777718, 0.00764537);
  m.rho_s = float3(0.110657, 0.0980322, 0.100579);
  m.alpha = float3(0.0926904, 0.0935964, 0.0949975);
  m.pm = float3(0.14163, 0.148703, 0.143694);
  m.F_0 = float3(0.150251, 0.169418, 0.170457);
  m.F_1 = float3(0.100065, 0.113089, 0.114468);
  m.K_ap = float3(11.2419, 11.113, 10.993);
  m.sh_lambda = float3(4.3545, 4.3655, 4.31586);
  m.sh_c = float3(0.00464641, 0.00384785, 0.0046145);
  m.sh_k = float3(14.6751, 14.8089, 14.5436);
  m.sh_theta0 = float3(0.275651, 0.262317, 0.271284);
  return m;
}

Material SilverMetallicPaint() {
  Material m;
  m.rho_d = float3(0.0554792, 0.0573803, 0.0563376);
  m.rho_s = float3(0.121338, 0.115673, 0.10966);
  m.alpha = float3(0.029859, 0.0303706, 0.0358666);
  m.pm = float3(0.144097, 0.104489, 0.158163);
  m.F_0 = float3(1.03749e-14, 3.52034e-15, 4.41778e-12);
  m.F_1 = float3(-1, -1, -1);
  m.K_ap = float3(31.9005, 32.1514, 26.5685);
  m.sh_lambda = float3(6.08248, 5.89319, 5.95156);
  m.sh_c = float3(0.00761403, 0.0839948, 0.00138703);
  m.sh_k = float3(23.5891, 20.6786, 23.3297);
  m.sh_theta0 = float3(0.435405, 0.54017, 0.34665);
  return m;
}

Material TwoLayerGold() {
  Material m;
  m.rho_d = float3(0.0415046, 0.0312801, 0.0253658);
  m.rho_s = float3(1.58161, 1.18736, 1.63847);
  m.alpha = float3(0.0263104, 0.0293804, 0.0241265);
  m.pm = float3(0.355682, 0.354281, 0.36415);
  m.F_0 = float3(0.117355, 0.0614942, 0.0447004);
  m.F_1 = float3(0.0411678, -0.0237579, -0.0100488);
  m.K_ap = float3(30.4478, 27.5367, 32.7278);
  m.sh_lambda = float3(3.91606, 3.99968, 3.79114);
  m.sh_c = float3(9.38513e-08, 6.62806e-08, 6.79906e-08);
  m.sh_k = float3(36.9091, 35.6578, 38.4915);
  m.sh_theta0 = float3(0.0815181, 0.0465613, 0.0922721);
  return m;
}

static const float kOnePi = 0.31830988618f;
#if 1
float3 Fresnel(float3 F0, float3 F1, float V_H) {
  return F0 - V_H * F1  + (1.f - F0)*pow(1. - V_H, 5.f);
}

float3 D(float3 _alpha, float3 _p, float cos_h, float3 _K) {
  float cos2 = cos_h*cos_h;
  float tan2 = (1.f-cos2)/cos2;
  float3 ax = _alpha + tan2/_alpha;
  ax = max(ax,0.f); //bug?
  return kOnePi * _K * exp(-ax)/(pow(ax,_p) * cos2 * cos2);
}

float3 G1(Material m, float theta) {
  theta = clamp(theta,-1.f,1.f);
  return 1.f + m.sh_lambda * (1.f - exp(m.sh_c * pow(max(acos(theta) - m.sh_theta0,0.f), m.sh_k)));
}

float3 Shade(Material m, float inLight, float n_h, float n_l, float n_v, float v_h) {
  return  kOnePi * inLight * (n_l * m.rho_d	
      + m.rho_s * D(m.alpha, m.pm, n_h, m.K_ap) * G1(m,n_l) * G1 (m,n_v) * Fresnel(m.F_0, m.F_1, v_h));
}

float3 BRDF(Material m, float3 lv, float3 ev, float3 n, float3 intensity) {
  float3 halfVector = normalize(lv + ev);
  float v_h = dot(ev, halfVector);
  float n_h = dot(n, halfVector);
  float n_l = dot(n, lv);
  float inLight = 1.f;
  if (n_l < 0.f) inLight = 0.f;
  float n_v = clamp(dot(n, ev), 0.f, 1.f);
  float3 sh = Shade(m, inLight, n_h, n_l, n_v, v_h);
  sh = clamp(intensity*sh, 0.f, 1.f);
  return sh;
}
#else

float3 Fresnel(float3 F0, float3 F1, float V_H)
{
	return F0 - V_H * F1  + (1. - F0)*pow(1. - V_H, 5.);
}

float3 D(float3 _alpha, float3 _p, float cos_h, float3 _K)
{
  float cos2 = cos_h*cos_h;
  float tan2 = (1.f-cos2)/cos2;
  float3 ax = _alpha + tan2/_alpha;
  ax = max(ax,0.f); //bug?
  return kOnePi * _K * exp(-ax)/(pow(ax,_p) * cos2 * cos2);
}

float3 G1(Material m, float theta)
{
	theta = clamp(theta,-1.,1.); //bug?
	return 1.0 + m.sh_lambda * (1. - exp(m.sh_c * pow(max(acos(theta) - m.sh_theta0,0.), m.sh_k)));
}

float3 shade(Material m, float inLight, float n_h, float n_l, float n_v, float v_h)
{
  return  kOnePi * inLight * (n_l * m.rho_d	
      + m.rho_s * D(m.alpha, m.pm, n_h, m.K_ap) * G1(m,n_l) * G1 (m,n_v) * Fresnel(m.F_0, m.F_1, v_h));
}

float3 BRDF(Material m, float3 lv, float3 ev, float3 n, float3 intensity)
{
	float3 halfVector = normalize(lv + ev);
	
	float v_h = dot(ev, halfVector);
	float n_h = dot(n, halfVector);
	float n_l = dot(n, lv); 
	float inLight = 1.0;
	if (n_l < 0.) inLight = 0.0;
	float n_v = dot(n, ev); 
	
	float3 sh = shade(m, inLight, n_h, n_l, n_v, v_h);
	sh = clamp(intensity*sh, 0., 1.); //bug?
	float3 retColor = sh;
	return retColor;
}
#endif
#endif

//------------------------------------------------------------------------
// Camera
//
// Move the camera. In this case it's using time and the mouse position
// to orbitate the camera around the origin of the world (0,0,0), where
// the yellow sphere is.
//------------------------------------------------------------------------
void DoCamera(out float3 camPos, out float3 camTar, in float time, in float mouseX)
{
  float an = 0.3*GLOBALTIME + 10.0*mouseX;
  //camPos = float3(3.5*sin(an),1.0,3.5*cos(an));
  camPos = float3(1.f,1.2f,2.2f);
  camTar = float3(0.0,0.0,0.0);
}

//------------------------------------------------------------------------
// Background
//
// The background color. In this case it's just a black color.
//------------------------------------------------------------------------
float3 doBackground(void) { return float3(0.0, 0.0, 0.0); }

float hash(float n) { return frac(sin(n)*43758.5453123f); }
float noise(in float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return lerp(lerp(lerp(hash(n+  0.0), hash(n+  1.0),f.x),
                     lerp(hash(n+157.0), hash(n+158.0),f.x),f.y),
                lerp(lerp(hash(n+113.0), hash(n+114.0),f.x),
                     lerp(hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}
float U(float d1, float d2) { return min(d1,d2); }
float S(float d1, float d2) { return max(-d1,d2); }
float I(float d1, float d2) { return max(d1,d2); }

float2 mod(float2 x, float2 y) { return x - y*floor(x/y); }
float3 mod(float3 x, float3 y) { return x - y*floor(x/y); }
float3 Rep(float3 p, float3 c) {
  float3 q = mod(p,c)-0.5f*c;
  return float3(q.x,q.y,p.z);
}

float Sphere(float3 p) { return length(p) - 1.0; }
float Box(float3 p, float3 b) {
  float3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float Model(float3 p) {
  //return U(Sphere(p), Box(Rep(p, float3(3.f,3.f,1000.f)), float3(1.f,1.f,1.f))) + 0.3f*noise(3.f*p) + 0.05*noise(100/.f*p);
  p = p + 0.2f*cos(GLOBALTIME);
  return Sphere(p+0.022f*abs(cos(50.f*p)));
}

//------------------------------------------------------------------------
// Material
//
// Defines the material (colors, shading, pattern, texturing) of the model
// at every point based on its position and normal. In this case, it simply
// returns a constant yellow color.
//------------------------------------------------------------------------
float3 DoMaterial(in float3 pos, in float3 nor)
{
  return float3(0.2,0.07,0.01);
}

//------------------------------------------------------------------------
// Lighting
//------------------------------------------------------------------------
float calcSoftshadow(in float3 ro, in float3 rd);

float3 DoLighting(in float3 pos, in float3 nor, in float3 rd, in float dis, in float3 mal)
{
  float3 lin = float3(0.f, 0.f, 0.f);
#if 0
  // key light
  //-----------------------------
  float3  lig = normalize(float3(1.0,0.7,0.9));
  float dif = max(dot(nor,lig),0.0);
  float sha = 0.0; if(dif>0.01) sha=calcSoftshadow(pos+0.01*nor, lig);
  lin += dif*float3(4.00,4.00,4.00)*sha;

  // ambient light
  //-----------------------------
  lin += float3(0.50,0.50,0.50);


  // surface-light interacion
  //-----------------------------
  float3 col = mal*lin;


  // fog
  //-----------------------------
  col *= exp(-0.01*dis*dis);
  return col;
#else
  float3  lig = normalize(float3(1.0,0.7,0.9+cos(GLOBALTIME)));
  //return BRDF(Aluminium(), lig, -rd, nor, 3.f*float3(10.f,10.f,10.f));
  //return BRDF(SilverMetallicPaint(), lig, -rd, nor, 3.f*float3(10.f,10.f,10.f));
  //return BRDF(BlackFabric(), lig, -rd, nor, 3.f*float3(10.f,10.f,10.f));
  return BRDF(TwoLayerGold(), lig, -rd, nor, 3.f*float3(10.f,10.f,10.f));
  //return BRDF(BlackSoftPlastic(), lig, -rd, nor, 3.f*float3(10.f,10.f,10.f));
#endif
}

float calcIntersection(in float3 ro, in float3 rd)
{
  const float maxd = 20.0;       // max trace distance
  const float precis = 0.001;    // precission of the intersection
  float h = precis*2.0;
  float t = 0.0;
  float res = -1.0;
  for(int i=0; i<90; i++)      // max number of raymarching iterations is 90
  {
    if(h<precis||t>maxd) break;
    h = Model(ro+rd*t);
    t += h;
  }

  if(t<maxd) res = t;
  return res;
}

float3 calcNormal(in float3 pos)
{
  const float eps = 0.002;       // precision of the normal computation
  const float3 v1 = float3(1.0,-1.0,-1.0);
  const float3 v2 = float3(-1.0,-1.0, 1.0);
  const float3 v3 = float3(-1.0, 1.0,-1.0);
  const float3 v4 = float3(1.0, 1.0, 1.0);
  return normalize(v1*Sphere(pos + v1*eps) +
                   v2*Sphere(pos + v2*eps) +
                   v3*Sphere(pos + v3*eps) +
                   v4*Sphere(pos + v4*eps));
}

float calcSoftshadow(in float3 ro, in float3 rd)
{
  float res = 1.0;
  float t = 0.0005;         // selfintersection avoidance distance
  float h = 1.0;
  for(int i=0; i<40; i++)     // 40 is the max numnber of raymarching steps
  {
    h = Sphere(ro + rd*t);
    res = min(res, 64.0*h/t);   // 64 is the hardness of the shadows
    t += clamp(h, 0.02, 2.0);   // limit the max and min stepping distances
  }
  return clamp(res,0.0,1.0);
}

float3x3 LookAtMatrix(in float3 ro, in float3 ta, in float roll)
{
  float3 ww = normalize(ta - ro);
  float3 uu = normalize(cross(ww,float3(sin(roll),cos(roll),0.0)));
  float3 vv = normalize(cross(uu,ww));
  return float3x3(uu, vv, ww);
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
  float2 gl_FragCoord = float2(DTid.xy);
  float2 p = (-RESOLUTION.xy + 2.0*gl_FragCoord.xy)/RESOLUTION.y;
  float2 iMouse = float2(0.f,0.f);
  float2 m = iMouse.xy/RESOLUTION.xy;

  //-----------------------------------------------------
  // camera
  //-----------------------------------------------------

  // camera movement
  float3 ro, ta;
  DoCamera(ro, ta, GLOBALTIME, m.x);

  // camera matrix
  float3x3 camMat = LookAtMatrix(ro, ta, 0.0);  // 0.0 is the camera roll

//  ro += float3(0.f,1.f,0.f);
  // create view ray
  float3 rd = normalize(mul(camMat, float3(p.xy,2.0))); // 2.0 is the lens length

  //-----------------------------------------------------
  // render
  //-----------------------------------------------------

  float3 col = doBackground();

  // raymarch
  float t = calcIntersection(ro, rd);
  if(t>-0.5)
  {
    // geometry
    float3 pos = ro + t*rd;
    float3 nor = calcNormal(pos);

    // materials
    float3 mal = DoMaterial(pos, nor);

    col = DoLighting(pos, nor, rd, t, mal);
  }

  //-----------------------------------------------------
  // postprocessing
  //-----------------------------------------------------
  // gamma
  float3 c = pow(clamp(col,0.0,1.0), float3(0.4545f,0.4545f, 0.4545f));

  //  output[DTid.xy] = float4(float3(c-t*0.0375f+p.y*0.05f,c-t*0.025f-p.y*0.0625f,c+t*0.025f-p.y*0.025f)+color*color,1.0f);
  output[DTid.xy] = clamp(float4(c.x,c.y,c.z,1.f), 0.f, 1.f);
}
#endif
