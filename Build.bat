@echo off

SET CommonIncludeDirectories="D:\Tools\SDK\SDL2\SDL2\include";"D:\Tools\SDK\SDL2\SDL2_mixer\include";"D:\Tools\SDK\SDL2\SDL2_image\include";
SET SDLLibDirectory="..\..\Code\External\SDL2\lib\x64\SDL2.lib" "..\..\Code\External\SDL2_image\lib\x64\SDL2_image.lib" "..\..\Code\External\SDL2_mixer\lib\x64\SDL2_mixer.lib"
SET CommonCompilerFlags=-Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4189 -wd4100 -wd4996 -wd4201 -FC -Z7 /EHsc
SET CommonCompilerFlags=-D _ITERATOR_DEBUG_LEVEL=2 -D KANTI2D_INTERNAL=1 -D KANTI2D_SLOW=1 -D KANTI2D_WIN32=1 %CommonCompilerFlags%
SET CommonLinkerFlags=-incremental:no -opt:ref %SDLLibDirectory% /SUBSYSTEM:CONSOLE

IF NOT EXIST Build MKDIR Build
PUSHD Build
IF NOT EXIST 64 MKDIR 64
PUSHD 64
cl %CommonCompilerFlags% /I\CommonIncludeDirectories ..\..\Code\Kanti2DMain.cpp /link %CommonLinkerFlags%
POPD
POPD

@IF ERRORLEVEL == 0 (
   GOTO COMPILED
)

@IF ERRORLEVEL != 0 (
   GOTO CRASH
)

:COMPILED
   ECHO "Compiled Successfully"
   ECHO %ERRORLEVEL%
   GOTO END

:CRASH
   ECHO "error or warning"
   ECHO %ERRORLEVEL%
   GOTO END

:END