#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <psppower.h>

#include "cmlibmenu.h"

/*----------------------------------------------------------------------------*/

// モジュールの定義
PSP_MODULE_INFO( "SimpleBatteryViewer", PSP_MODULE_KERNEL, 1, 3 );

/*----------------------------------------------------------------------------*/
#define WHITE 0xFFFF
#define BLACK 0x8000
#define home_col 0x9ce7
#define check_col 0xdef7

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  INIT METHOD	
//  Thanks SnyFbSx , estuibal , hiroi01
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

typedef struct _tag_prxliblist {
    char *mod_name;
    char *prx_path;
} prxLibList;

int LoadStartModule(char *module)
{
	SceUID mod = sceKernelLoadModule(module, 0, NULL);

	if (mod < 0) return mod;

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
}

int loadLibraries( void )
{
	int i;
	SceIoStat stat;
	prxLibList pll[] = {
		{ "cmlibMenu",  "ms0:/seplugins/lib/cmlibmenu.prx"  }
	};
	
	while( sceKernelFindModuleByName( "sceKernelLibrary" ) == NULL ) sceKernelDelayThread( 1000 );

	for( i = 0; i < NELEMS(pll); i++ )
	{
		if( sceKernelFindModuleByName(pll[i].mod_name) == NULL )
		{
			if(sceIoGetstat(pll[i].prx_path, &stat) < 0)
			{
				pll[i].prx_path[0] = 'e'; pll[i].prx_path[1] = 'f';
				if(sceIoGetstat(pll[i].prx_path, &stat) < 0)continue;
			}
			LoadStartModule(pll[i].prx_path);
		}
	}
		
	return 0;
}

/*----------------------------------------------------------------------------*/

bool InitBuffers(libm_draw_info *dinfo )
{
	sceDisplayGetFrameBufferInternal(0, &dinfo->vinfo->buffer, &dinfo->vinfo->lineWidth, &dinfo->vinfo->format, 0);
	dinfo->convert	= NULL;
	dinfo->blend	= NULL;
	dinfo->vinfo->width = 480;
	dinfo->vinfo->height = 272;

	if( !dinfo->vinfo->buffer || !dinfo->vinfo->lineWidth || dinfo->vinfo->format != PSP_DISPLAY_PIXEL_FORMAT_5551) return false;
	
	dinfo->vinfo->pixelSize	= 2;
	dinfo->vinfo->lineSize	= dinfo->vinfo->lineWidth * dinfo->vinfo->pixelSize;
	dinfo->vinfo->frameSize	= dinfo->vinfo->lineSize * dinfo->vinfo->height;

	return true;
}

/*----------------------------------------------------------------------------*/

int main_thread( SceSize arglen, void *argp )
{
	libm_draw_info dinfo;
	libm_vram_info vinfo;
	int btryLifeP;
	int btryLifeT;
	char bat_info[16];
	
	while(sceKernelFindModuleByName("sceKernelLibrary") == NULL)
	{
		sceKernelDelayThread(1000);
	}
	
	loadLibraries();
	
	memset(&dinfo, 0, sizeof(dinfo));
	memset(&vinfo, 0, sizeof(vinfo));
	dinfo.vinfo = &vinfo;
	
	libmLoadFont(LIBM_FONT_CG);
	
	while(1){
		sceKernelDelayThread(100*1000);
		if(InitBuffers(&dinfo))
		{
			libmFillRect(390, 22, 480, 32, home_col, &dinfo);
			u16 *raw16 = (u16 *)(dinfo.vinfo->buffer + 902 + 20 * dinfo.vinfo->lineWidth);
			if(raw16[0] == check_col)
			{
				btryLifeP = scePowerGetBatteryLifePercent();
				btryLifeT = scePowerGetBatteryLifeTime();
				if(btryLifeP < 0) btryLifeP = 0;
				if(btryLifeT < 0) btryLifeT = 0;
				sprintf(bat_info, "%3d%%(%2d:%02d)", btryLifeP, btryLifeT/60, btryLifeT%60);
				libmPrintXY(391, 23, BLACK, 0, bat_info, &dinfo);
				libmPrintXY(392, 23, BLACK, 0, bat_info, &dinfo);
				libmPrintXY(391, 24, BLACK, 0, bat_info, &dinfo);
				libmPrintXY(390, 22, WHITE, 0, bat_info, &dinfo);
			}
		}
	}
	
	return 0;
}

/*----------------------------------------------------------------------------*/

int module_start( SceSize arglen, void *argp )
{
	if(sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_GAME)
	{
		SceUID thid = sceKernelCreateThread( "main_thread", main_thread, 32, 0x1000, 0, 0 );
		if( thid ) sceKernelStartThread( thid, arglen, argp );
	}
	return 0;
}

int module_stop( void )
{
	return 0;
}


