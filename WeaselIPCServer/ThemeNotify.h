#pragma once

#include "WeaselServerImpl.h"

BOOL InitThemeNotifier(weasel::ServerImpl* server);
void ReadColorMode();

extern DWORD SysThemeIsLight;
extern DWORD AppThemeIsLight;
