$game_path = "D:\Ubisoft\Heroes of Might and Magic V - Tribes of the East"

Move-Item "Release\H5_DLL.dll" "um.dll" -Force
Robocopy.exe "." "$game_path\bin" "um.dll" /Z /mov
#Read-Host