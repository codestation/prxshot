prxshot v0.4 - Codestation

This Homebrew is an entry for the PSP Genesis competition 2011,
sponsored by npt, psp-hacks.com, pspgen.com, pspslimhacks.com,
exophase.com, pspcustomfirmware.com, daxhordes.org, gamegaz.jp,
xtreamlua.com and wololo.net

This is a screenshot plugin that doesn't need to pause the game to take
the capture (useful for online gaming) and classifies the images
per game using their name and icons.

The plugin stores the screenshot in BMP format inside:
>>  /PSP/SCREENSHOT/<GAME_ID>/pic_XXXX.bmp

for umd/iso games or:
>>  /PSP/SCREENSHOT/PS<8 bytes sha1sum of the title>/pic_XXXX.bmp

for homebrew or games in PBP format (because the homebrews have
the same GAME_ID).

Installation:
Copy the prxshot directory to the seplugins directory (or anywhere you want)
and add the plugin to the game.txt file (and/or vsh.txt if you want to).

Note: prxshot++.prx is also included with this release, this is the c++ port
of my plugin and is gonna be the default in later releases. For now the c and
c++ versions have _exactly_ the same functionality so doesn't matter what one
do you use. I included both in this version just because the C version is more
tested. If you want to help fixing bugs, please use the c++ version when
reporting them.

Note for developers: this source code is a good starting point if you want
to use OOP paradigm that c++ offers in your kernel code. I am gonna continue
to improve it in later releases (like including some STL classes and implementing
some sort of exception support if possible).

How to use it: press the NOTE button to take a screenshot (or configure it
to the key(s) that you want)

The prxshot directory comes with these files:
* prxshot.prx: the plugin
* default_icon0.png: optional default icon to use for those homebrews
  without icon (you can make your own image, just try to maintain the
  same dimensions)
* xmb.sfo: template to make folder icon for screenshots in VSH mode.
* prxshot.ini: configuration file

Meaning of the options in the prxshot.ini:

"General" category:

* CreatePic1: 0 by default. Change to 1 if you want the game background image
  that appears in the XMB in the screenshot folder.
  
* PSPGoUseMS0: 0 by default. Change to 1 if you have a PSP Go and want to store
  the pictures into the M2 instead of the internal storage.
  
* XMBClearCache: 0 by default. Change to 1 if you want to refresh the cache
  after taking a screenshot in the XMB (solves the problem of the picture 
  viewer that doesn't show the new screenshots but can cause freezes when 
  using Game Categories)
  
* ScreenshotKey: 0x800000 by default. Change it to one of the values of the
  table below (or a combination of them) to select the button(s) required for
  the screenshot.
  
* KeyTimeout: 0 by default. Change it to a positive value (in millisecs) to
  delay the screenshot key(s) to that value. For example, setting it to 1000
  causes the screenshot to be taken 1 second after the key was pressed
  
* ScreenshotName: %s/pic_%04d.bmp by default. Here you can specify a different
  prefix/suffix for the screenshot. Important: the "%s/, "%04d" and the ".bmp"
  are mandatory in that order.
  
"CustomKeys" category:

* ULJM08500 (a.k.a. game id): Undefined by default. Here you can use a game id
 (you can obtain them from your game box, playstation site, UMDGen, ISO Tool,
 PSPFiler (reading the UMD_DATA.BIN in the iso/umd), cwcheat, etc, etc) and a
 screenshot key that you want to link to that game. Examples:
 
 ULJM05800 = 0x000009 (Monster Hunter 3rd game using SELECT + START to take
                       the screenshot)
                       
 ULES01505 = 0x000101 (Dissidia Duodecim using SELECT + LTRIGGER to take
                       the screenshot)
                       
"CustomTimeout" category:

* ULJM08500 (a.k.a. game id): Undefined by default. Here you can use a game id
 (you can obtain them from your game box, playstation site, UMDGen, ISO Tool,
 PSPFiler (reading the UMD_DATA.BIN in the iso/umd), cwcheat, etc, etc) and a
 timeout for that game (see KeyTimeout definition for details). Examples:
 
 ULJM05800 = 300 (Monster Hunter 3rd game using 300 msecs to take a screenshot)
 
 ULES01505 = 1500 (Dissidia Duodecim using 1.5 secs before taking the screenshot)
                       
# Values for the screenshot button

# Select button        0x000001
# Start button         0x000008
# Up D-Pad button      0x000010
# Right D-Pad button   0x000020
# Down D-Pad button    0x000040
# Left D-Pad button    0x000080
# Left trigger         0x000100
# Right trigger        0x000200
# Triangle button      0x001000
# Circle button        0x002000
# Cross button         0x004000
# Square button        0x008000
# Hold button          0x020000
# Music Note button    0x800000
# Screen button        0x400000
# Volume up button     0x100000
# Volume down button   0x200000

Changelog:
v0.4:
[+]Implemented key timeout, so now you can add some delay to the screenshot
button so doesn't interfere with the normal game operation.
[!]Internal change: rewrote the plugin in C++
[!]Fixed the known issue of taking a screenshot while the aLoader is
launching an ISO (only for the C++ branch)
v0.3:
[+]Added the option to set your custom screenshot key per game.
[!]Major rewrite to the hook core (removed the asm payload code), so it avoids
the crashes in 6.20 TN + loaders (i hate those loaders, really >_<).
[!]Moved the comments of the prxshot.ini to the readme for more readibility.
v0.2.4:
[!]Fixed the bug introduced in 0.2.3 that didn't detect the settings
in the .ini file.
[!]Fixed bug with homebrew not properly detected in PSPGo using the
internal storage.
v0.2.3:
[+]Added support for eLoader (1.50 eboot loader)
[+]Recreate the XMB folder if is deleted.
[!]Fixed nasty bug that causes a crash when taking very early screenshots
(Note that you can't take screenshots on loaders because the game isn't
loaded yet, a special folder is gonna be added for this in the future)
[+]Added option XMBClearCache to the prxshot.ini. While this updates the picture
directory when taking screenshots in the XMB i recommend that is better
to leave it disabled because it causes troubles with game categories.
Added more debug messages.
v0.2.2:
[+]Added support for POPS (PSone games support).
[+]Added support for storing the screenshots into the M2 in the PSPGo.
[+]Added logging and made it conditional on build when compiling from source.
[!]Fixed bug with XMB icon creation introduced in 0.2.0.
v0.2.1:
[+]Added OIL to blacklist.
v0.2.0:
[!]Removed trailing "/" so the directory creation don't fail.
[+]New configuration file (prxshot.ini) so now you can configure some things
like background use for picture folders, change the button combo to take
the screenshot and the suffix/prefix of the picture file.
v0.1.3:
[+]Create /PSP/SCREENSHOT in case it doesn't exists
v0.1.2:
[!]Rewrote asm payload code (doesn't need memory allocation anymore so hopefully
the issues with 6.20 / aLoader are finally fixed)
v0.1.1:
[!]Fixes bugs with 6.20 TN and/or aLoader
v0.1.0:
[+]First release

Source code available at https://github.com/codestation/prxshot
