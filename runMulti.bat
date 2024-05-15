@ECHO OFF
SET "proj=\Unreal_Ink_Shooter.uproject
SET "path=%cd%%proj%"
echo %path%
"C:\Program Files\Epic Games\UE_5.2\Engine\Binaries\Win64\UnrealEditor.exe" "%path%" -game -windowed -ResX=800 -ResY=900 -WinX=20 -WinY=20 -log
pause