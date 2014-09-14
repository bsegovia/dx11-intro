#include "sound.hpp"
static const float PI=3.14159265358979323846f;
static const float TAU=2.f*PI;

RWStructuredBuffer<int> g_soundTrack : register(u0);

float2 music(float t) { return float2(sin(TAU*440.f*t),sin(TAU*440.f*t)); }

[numthreads(1024, 1, 1)]
void main(uint2 tid : SV_DispatchThreadID) {
  float2 flr = music(float(tid.x+tid.y*MZK_THREAD_NUM) / float(MZK_RATE));
  int2 ilr = int2(32767.f*flr) + 32767;
  g_soundTrack[tid.x+tid.y*MZK_THREAD_NUM] = ilr.x | (ilr.y << 16);
}

