#pragma once
#include <Windows.h>
#include "globals.h"

LRESULT CALLBACK WndProcProlog(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcEpilog(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ShellProcProlog(int nCode, WPARAM wParam, LPARAM lParam);