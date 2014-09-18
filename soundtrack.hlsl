#include "sound.hpp"
static const float PI=3.14159265358979323846f;
static const float TAU=2.f*PI;

RWStructuredBuffer<int> g_soundTrack : register(u0);

#if 0
float Envelope(float time, float decay) {	
  return exp2(-time * (5.0 / decay));
}

float Envelope(float time, float attack, float decay) {
  if (time < attack)
    return time/attack;
  time -= attack;
  return Envelope(time, decay);
}

float NoteToHz(float n) { return 440.0*pow(2.0, (n-69.0)/12.0); }
float Tri(float t) { return abs(frac(t) * 4.0 - 2.0) - 1.0; }
float Saw(float t) { return frac(t) * 2.0 - 1.0; }
float Cos(float t) { return cos(t * radians(360.0)); }
float Square(float t) { return step(frac(t), 0.5) * 2.0 - 1.0; }

float4 Cos4(float4 x) {
  x = abs(frac(x) * 2.0 - 1.0);
  float4 x2 = x*x;
  return x2 * 6.0 - x2*x * 4.0  - 1.0;
}
float4 Saw4(float4 x) { return frac(x) * 2.0 - 1.0; }

// Thanks to Dave_Hoskins for the hash
float Hash(float p) {
  float2 p2 = frac(float2(p * 5.3983, p * 5.4427));
  p2 += dot(p2.yx, p2.xy + float2(21.5351, 14.3137));
  return frac(p2.x * p2.y * 95.4337);
}

float Noise(float x) {
  return Hash(floor(x * 32.0)) * 2.0 - 1.0;
}

float SmoothNoise(float t) {
  float noiset = t * 32.0;
  float tfloor = floor(noiset);
  float ffrac = frac(noiset);

  float n0 = Hash(tfloor);
  float n1 = Hash(tfloor + 1.0);
  float blend = ffrac*ffrac*(3.0 - 2.0*ffrac);
  return lerp(n0, n1, blend) * 2.0 - 1.0;
}

float FBM(float t, float persistence) {
  float result = 0.0;

  float a = 1.0;
  float tot = 0.0;
  result += SmoothNoise(t) * a; tot += a; t *= 2.02; a *= persistence;
  result += SmoothNoise(t) * a; tot += a; t *= 2.02; a *= persistence;
  result += SmoothNoise(t) * a; tot += a; t *= 2.02; a *= persistence;
  result += SmoothNoise(t) * a; tot += a; t *= 2.02; a *= persistence;
  tot += a;
  return result / tot;
}

float StepNoise(float t, float freq) {
  float noiset = t * freq;
  float tfloor = floor(noiset);
  float n = Hash(tfloor);
  return n * 2.0 - 1.0;
}

float Cos4(float x, float4 phase, float4 freq, float4 amp) {
  return dot(Cos4((x+phase) * freq), amp);
}

float Saw4(float x, float4 phase, float4 freq, float4 amp) {
  return dot(Saw4((x+phase) * freq), amp);
}

#define N(TIME,NOTE) t+=float(TIME); if(x>t) r=float2(NOTE,t);
float2 GetTrack1Note(float x) {
  float2 r = float2(-1.f,-1.f);
  float t = 0.f;
//  N(0,60)
#if 1
    N(24,40)N(28,52)N(24,40)N(26,40)N(26,50)N(24,40)N(26,40)N(26,48)N(24,40)
  N(26,40)N(26,46)N(24,40)N(28,40)N(24,47)N(26,48)N(26,40)N(24,40)N(26,52)N(26,40)
  N(24,40)N(26,50)N(26,40)N(24,40)N(28,48)N(24,40)N(26,40)N(26,46)N(126,40)
  N(26,40)N(24,52)N(28,40)N(24,40)N(26,50)N(26,40)N(24,40)N(26,48)N(26,40)
  N(24,40)N(26,46)N(26,40)N(24,40)N(28,47)N(24,48)N(26,40)N(26,40)N(24,52)
  N(26,40)N(26,40)N(24,50)N(26,40)N(26,40)N(24,63)N(14,60)N(14,59)N(12,63)
  N(12,66)N(12,64)N(14,63)N(12,59)N(14,63)N(12,64)N(12,66)N(12,67)N(14,66)
  N(14,64)N(12,63)N(12,59)N(12,40)N(26,40)N(26,52)N(24,40)N(28,40)N(24,50)
  N(26,40)N(26,40)N(24,48)N(26,40)N(26,40)N(24,46)N(26,40)N(26,40)N(24,47)
  N(28,48)N(24,40)N(26,40)N(26,52)N(24,40)N(26,40)N(26,50)N(24,40)N(26,40)
  N(26,48)N(24,40)N(28,40)N(24,46)N(128,40)N(24,40)N(26,52)N(26,40)N(24,40)
  N(28,50)N(24,40)N(26,40)N(26,48)N(24,40)N(26,40)N(26,46)N(24,40)
  N(26,40)N(26,47)N(24,48)N(28,40)N(24,40)N(26,52)N(26,40)N(24,40)N(26,50)
  N(26,40)N(24,40)N(26,67)N(14,64)N(12,59)N(12,64)N(12,67)N(14,64)N(14,67)N(12,71)N(12,67)N(12,64)
#endif
  return r;
}

#define kMidiTimebase 200.0f
#define kInvMidiTimebase (1.0f / kMidiTimebase)

float2 GetNoteData( float2 vMidiResult, float fMidiTime )
{
  return float2( NoteToHz(vMidiResult.x), (fMidiTime - vMidiResult.y) * kInvMidiTimebase );
}

float Test( float t )
{
  return Saw4(t, float4(0.0, 0.5, 0.1, 0.4), float4(1.0, 1.50, 2.00, -3.00), float4(1.0, 0.5, 0.25, 0.125));
}

float Test2(float f)
{
  return  Test(f) ;//+ Test(f - 0.0454) * 0.4 + Test(f - 0.1123) * 0.3 + Test(f - 0.1523) * 0.1;
}
float Instrument( float2 vFreqTime )
{
  return Test2( vFreqTime.x * vFreqTime.y ) * Envelope( vFreqTime.y, 0.01, 1.0 ); 
}

float Track1Instrument( float2 vFreqTime )
{
  return Instrument( vFreqTime * float2(2.0, 1.0) ) * 0.75;
}

float PlayMidi( float time ) {
  if(time < 0.0)
    return 0.0;

  float fMidiTime = time * kMidiTimebase;

  float fResult = 0.0;

  fResult += Track1Instrument( GetNoteData( GetTrack1Note(fMidiTime), fMidiTime ) );
  //   fResult += Track2Instrument( GetNoteData( GetTrack2Note(fMidiTime), fMidiTime ) );
  //   fResult += Track3Instrument( GetNoteData( GetTrack3Note(fMidiTime), fMidiTime ) );
  //   fResult += Track4Instrument( GetNoteData( GetTrack4ANote(fMidiTime), fMidiTime ) );
  //   fResult += Track4Instrument( GetNoteData( GetTrack4BNote(fMidiTime), fMidiTime ) );
  //   fResult += Track4Instrument( GetNoteData( GetTrack4CNote(fMidiTime), fMidiTime ) );
  //   fResult += Track4Instrument( GetNoteData( GetTrack4DNote(fMidiTime), fMidiTime ) );
  //   fResult += Track4Instrument( GetNoteData( GetTrack4ENote(fMidiTime), fMidiTime ) );
#if 0    
  fResult = clamp(fResult * 0.2, -1.0, 1.0);

  float fFadeEnd = 20.0 * 240.0 / kMidiTimebase;
  float fFadeTime = 5.0;
  float fFade = (time - (fFadeEnd - fFadeTime)) / fFadeTime;    
  fResult *= clamp(1.0 - fFade, 0.0, 1.0);
#endif  
  return fResult;
}
float2 music(float t) { float x = 1.0*PlayMidi(t); return float2(x,x); }
#else
float2 music(float t) { return float2(0.f,0.f); }
//float2 music(float t) { return float2(sin(fmod(TAU*440.f*t,TAU)),sin(fmod(TAU*440.f*t,TAU))); }
#endif

[numthreads(1024, 1, 1)]
void main(uint2 tid : SV_DispatchThreadID) {
  float2 flr = music(float(tid.x+tid.y*MZK_THREAD_NUM) / float(MZK_RATE));
  int2 ilr = int2(32767.f*flr) & 0xffff;
  g_soundTrack[tid.x+tid.y*MZK_THREAD_NUM] = ilr.x | (ilr.y << 16);
}

