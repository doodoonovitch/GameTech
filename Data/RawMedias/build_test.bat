@echo off
SET binDir=..\..\Output\bin\x64\Release
SET buildData=%binDir%\DataBuilder.exe
SET srcDir=Test
SET trgDir=Test

FOR %%F IN (Bind_Before_Switch Camera humanoid JointHierarchy Normals Sadface StereoCamera apple girl Sonya WonderWoman AlanTree Anna_Summer treeplan1 katana Talia oak-a Sword) DO (
	echo Building %%F.fbx ...
	%buildData% %srcDir%\%%F.fbx %trgDir%\%%F.xml
	echo ... end (%%F.fbx) 
)

PAUSE



