#pragma once
#define WELLBEHAVIOUR
// #define NAKED_ENTRY
// #define RECOMPILE_SHADER
#define RECOMPILE_SHADER 1
#define D3DRUN(X) do {\
  const auto hr = X;\
  if (hr != S_OK)\
    MessageBoxA(NULL, (LPCSTR) #X, "Error", MB_OK | MB_ICONERROR);\
  } while (0)
#include "intro.cpp"

