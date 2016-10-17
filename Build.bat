@echo on

SET CommonIncludeDirectories="D:\Tools\SDK\SDL2-2.0.4\include"
SET SDLLibDirectory="D:\Tools\SDK\SDL2-2.0.4\lib\x64\SDL2.lib"
SET CommonCompilerFlags=-Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4189 -wd4100 -wd4996 -wd4201 -FC -Z7 /EHsc
SET CommonCompilerFlags=-D _ITERATOR_DEBUG_LEVEL=2 -D KANTI2D_INTERNAL=1 -D KANTI2D_SLOW=1 -D KANTI2D_WIN32=1 %CommonCompilerFlags% 
SET CommonLinkerFlags=-incremental:no -opt:ref %SDLLibDirectory% /SUBSYSTEM:CONSOLE

IF NOT EXIST Build mkdir Build
pushd Build
cl %CommonCompilerFlags% /I %CommonIncludeDirectories% ..\Code\Kanti2DMain.cpp /link %CommonLinkerFlags%
popd