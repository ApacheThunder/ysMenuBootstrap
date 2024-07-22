# YSMENU Bootstrap

This is a modified version of nds-hb-menu's bootstrap setup to use specific arm9/arm7 entry points YSMenu expects for ttdldi.dat and ttpatch.dat. (these files are used to load the NDS selected NDS file)

This bootstrap parses TTMENU.SYS file created by YSMenu to find the path of the NDS file end use is trying to launch.

The purpose of this program is to allow booting homebrew from YSMenu on unsupported cards. (like N-Card/N-Card Clones for example)

This does NOT allow booting of retail games on unsupported flashcarts. One possible way to do that is to create a version for ttpatch.dat that redirects the file path to use to something like TWiLight Menu's nds-bootstrap. I may or may not do this in the future if there's popular demand for it.


Note to modify YSMenu to accept your flashcart as a supported cart. Open YSMenu NDS file in hex editor. Search for the string "TTIO". There should only be one instance grouped in with other DLDI codes like R4TF and such. Edit that string to be the 4 digit letter/code for the DLDI you will be using with YSMenu.

This will allow YSMenu to think your cart is supported and will have it expect ttdldi.dat and ttpatch.dat for launching NDS files. Then also dldipatch the ttdldi.dat and ttpatch.dat file from this repo to also use the dldi intended for your cart.

Ensure TTMENU.sys file exists. YSMenu might hang trying to create it so you may need to have a copy of this file present first.


# Disclaimer

Note some flashcarts are known to brick when using unsupported YSMenu versions on them. Do note I have made no attempts to fix this. The replacement of the ttpatch.dat file might prevent this but I do not know how or why the bricking happens and if that brick code isn't happening in the dat files I replace then you can still brick the flash cart. Use this at your own risk.

This is safe to use on N-Cards however as I have tested this on those.


# License
Note: While the GPL license allows you to distribute modified versions of this program it would be appreciated if any improvements are contributed to devkitPro. Ultimately the community as a whole is better served by having a single official source for tools, applications and libraries.

The latest sources may be obtained from devkitPro git using the command: `git clone git@github.com:devkitPro/nds-hb-menu.git`

```
 Copyright (C) 2005 - 2017
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy

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
 ```
