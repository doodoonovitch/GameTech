@echo off
SET binDir=..\Tools
SET exe=%binDir%\ImportScene.exe
SET srcDir=Test
SET trgDir=Test

FOR %%F IN (Bind_Before_Switch Camera humanoid JointHierarchy Normals Sadface StereoCamera apple girl Sonya WonderWoman AlanTree Anna_Summer treeplan1 katana Talia oak-a Sword) DO (
	echo Building %%F.fbx ...
	%exe% %srcDir%\%%F.fbx >> %trgDir%\%%F.txt
	echo ... end (%%F.fbx)
)

PAUSE



