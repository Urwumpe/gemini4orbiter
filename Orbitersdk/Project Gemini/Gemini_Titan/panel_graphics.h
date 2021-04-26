#ifndef __PANEL_GRAPHICS_H
#define __PANEL_GRAPHICS_H

typedef struct {
	HINSTANCE hDLL;
	HFONT font[2];
	DWORD col[4];
	HBRUSH brush[5];
	HPEN pen[4];
} GDIParams;

GDIParams g_Param;


#endif 
