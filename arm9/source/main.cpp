/*-----------------------------------------------------------------
 Copyright (C) 2005 - 2013
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy
	Claudio "sverx"

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------------------------------------*/
#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <fat.h>
#include <sys/stat.h>
#include <limits.h>

#include <string.h>
#include <unistd.h>

#include "font.h"
#include "tonccpy.h"
#include "inifile.h"
#include "nds_loader_arm9.h"

using namespace std;

const bool IsPatchBuild = false;

volatile bool guiEnabled = false;
volatile bool sysFileError = false;

static int bg, bgSub;

#define BUFFSIZE 0x300
#define PATHSIZE 0x2FF

static PrintConsole tpConsole;
static PrintConsole btConsole;

// char pathBuffer[0x100];
u8 fileBuffer[BUFFSIZE];
u8 savBuffer[BUFFSIZE];

const char* ttMenuPath = "/TTMENU.SYS";
const char* BOOTSTRAP = "/_nds/nds-bootstrap-release.nds";
const char* BOOTSTRAP_INI = "/_nds/nds-bootstrap.ini";

char* currentFilePath[PATHSIZE];
char* currentSavFilePath[PATHSIZE];


void InitGUI(void) {
	if (guiEnabled)return;
	guiEnabled = true;
	videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);
	vramSetBankA (VRAM_A_MAIN_BG);
	vramSetBankC (VRAM_C_SUB_BG);
	
	bg = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 1, 0);
	bgSub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 1, 0);
		
	consoleInit(&btConsole, 3, BgType_Text4bpp, BgSize_T_256x256, 20, 0, false, false);
	consoleInit(&tpConsole, 3, BgType_Text4bpp, BgSize_T_256x256, 20, 0, true, false);
		
	ConsoleFont font;
	font.gfx = (u16*)fontTiles;
	font.pal = (u16*)fontPal;
	font.numChars = 95;
	font.numColors =  fontPalLen / 2;
	font.bpp = 4;
	font.asciiOffset = 32;
	font.convertSingleColor = true;
	consoleSetFont(&btConsole, &font);
	consoleSetFont(&tpConsole, &font);

	consoleSelect(&btConsole);
}

int stop(bool showMessage, const char* Message) {
	if (showMessage) {
		InitGUI();
		consoleClear();
		printf(Message);
	}
	while (1) {
		swiWaitForVBlank();
		scanKeys();
		if (keysDown() != 0)break;
	}
	return 0;
}

/*int findStringlength (u8* strbuffer, int buffsize) {
	int i;
	for (i = 0; i < buffsize; i++) {
		if (strbuffer[i] == 0)return i;
	}
	return i;
}*/


void buildFATPath (char* destBuffer, char* sourceBuffer, int size) {
	int i;
	// Add fat: string to start of path as required by nds-bootstrap.
	destBuffer[0] = 'f';
	destBuffer[1] = 'a';
	destBuffer[2] = 't';
	destBuffer[3] = ':';
	for (i = 4; i < size; i++)destBuffer[i] = sourceBuffer[i - 4];
}

int main(int argc, char **argv) {
	extern u64 *fake_heap_end;
	*fake_heap_end = 0;
	
	defaultExceptionHandler();
	
	u32 ttMenuSize = 0;
	u32 pathSize = 0;
	u32 savPathSize = 0;
	
	if (!fatInitDefault())return stop(true, "\n\n\n\n\n\n\n\n\n\n\n        FAT init failed!\n");
	
	toncset((char*)currentFilePath, 0, PATHSIZE);
	
	if(IsPatchBuild) {
		toncset((char*)currentSavFilePath, 0, PATHSIZE);
		InitGUI();
		consoleClear();
		consoleSelect(&tpConsole);
		printf("\n\n\n\n\n\n\n\n\n\n\n          LOADING ...\n");
		consoleSelect(&btConsole);
		consoleClear();
	}	
	
	sysFileError = (access(ttMenuPath, F_OK) != 0);
	
	FILE *ttmenu = fopen(ttMenuPath, "rb");;
	
	if (!ttmenu) {
		sysFileError = true;
	} else {
		fseek(ttmenu, 0, SEEK_END);
		ttMenuSize = ftell(ttmenu);
		if (ttMenuSize < (BUFFSIZE + 0x1100)) {
			sysFileError = true;
		} else {
			fseek(ttmenu, 0x100, SEEK_SET);
			fread(fileBuffer, 1, BUFFSIZE, ttmenu);
			if (IsPatchBuild) {
				fseek(ttmenu, 0x1100, SEEK_SET);
				fread(savBuffer, 1, BUFFSIZE, ttmenu);
			}
			fclose(ttmenu);
		}
	}
	
	
	if (sysFileError)return stop(true, "\n\n\n\n\n\n\n\n\n\n\n       TTMENU.SYS ERROR!\n");
	
	/*savPathSize = findStringlength(fileBuffer, BUFFSIZE);
	pathSize = findStringlength(savBuffer, BUFFSIZE);*/
	
	pathSize = (strlen((const char*)fileBuffer) + 1);
	if (IsPatchBuild)savPathSize = (strlen((const char*)savBuffer) + 1);
	
	if ((pathSize == 0) || ((char)fileBuffer[0] != '/'))return stop(true, "\n\n\n\n\n\n\n\n\n\n\n          INVALID PATH!\n");
	if (IsPatchBuild) {
		if ((savPathSize == 0) || ((char)savBuffer[0] != '/'))return stop(true, "\n\n\n\n\n\n\n\n\n\n\n          INVALID PATH!\n");
		pathSize += 4;
		savPathSize += 4;
	}
		
	if ((pathSize < PATHSIZE)) {
		if (IsPatchBuild) {
			if (savPathSize > PATHSIZE)return stop(true, "\n\n\n\n\n\n\n\n\n\n\n       NDS PATH TOO LONG!\n");
			buildFATPath((char*)currentFilePath, (char*)fileBuffer, pathSize);
			buildFATPath((char*)currentSavFilePath, (char*)savBuffer, savPathSize);
		} else {
			tonccpy((char*)currentFilePath, (char*)fileBuffer, pathSize);
		}
	} else {
		return stop(true, "\n\n\n\n\n\n\n\n\n\n\n       NDS PATH TOO LONG!\n");
	}
	
	if ((access(BOOTSTRAP, F_OK) == 0) && IsPatchBuild) {
		if (access(BOOTSTRAP_INI, F_OK) == 0) {
			const char *argarray[] = { BOOTSTRAP };
			CIniFile bootstrapini(BOOTSTRAP_INI);
			bootstrapini.SetString("NDS-BOOTSTRAP", "NDS_PATH", (char*)currentFilePath);
			bootstrapini.SetString("NDS-BOOTSTRAP", "SAV_PATH", (char*)currentSavFilePath);
			bootstrapini.SetString("NDS-BOOTSTRAP", "HOMEBREW_ARG", "");
			bootstrapini.SetString("NDS-BOOTSTRAP", "RAM_DRIVE_PATH", "");
			bootstrapini.SetInt("NDS-BOOTSTRAP", "DSI_MODE", 0);
			bootstrapini.SetInt("NDS-BOOTSTRAP", "BOOST_CPU", 0);
			bootstrapini.SetInt("NDS-BOOTSTRAP", "BOOST_VRAM", 0);
			bootstrapini.SaveIniFile(BOOTSTRAP_INI);
			runNdsFile(BOOTSTRAP, 1, argarray);
		} else {
			return stop(true, "\n\n\n\n\n\n\n\n\n\n\n       INI FILE ERROR!\n");
		}
	} else if(access((const char*)currentFilePath, F_OK) == 0) {
		const char *argarray[] = { (char*)currentFilePath };
		runNdsFile((const char*)currentFilePath, 1, argarray);
	} else {
		return stop(true, "\n\n\n\n\n\n\n\n\n\n\n         NDS PATH ERROR!\n");
	}
	
	return stop(true, "\n\n\n\n\n\n\n\n\n\n\n        FILE BOOT ERROR!\n");
}

