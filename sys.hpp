#pragma once
#pragma function(memcpy)
#pragma function(memset)
static void* __cdecl memset(void* _Dst, _In_ int _Val, size_t _Size) {
  char *dst = (char*)_Dst;
  for (size_t x = 0; x < _Size; x++) *dst++ = _Val;
  return _Dst;
}

static void* __cdecl memcpy(void* _Dst, const void* _Src, size_t _Size) {
  char *dst = (char*)_Dst;
  char *src = (char*)_Src;
  for (size_t x = 0; x < _Size; x++) *dst++ = *src++;
  return _Dst;
}

#define INLINE __forceinline

