@echo off

REM 언리얼 엔진 설치 경로 (UnrealBuildTool 경로)
SET UBT_PATH=D:\UE_5.5\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe

REM 프로젝트 경로
SET PROJECT_PATH=D:\MOBA_Portfolio\WBA\WillBeAOS\WillBeAOS.uproject

REM 빌드할 대상 이름 (ProjectName)
SET TARGET=WillBeAOS

REM 서버 전용 빌드
"%UBT_PATH%" %TARGET% Win64 Development -Project="%PROJECT_PATH%" -TargetType=Server -Progress -NoHotReloadFromIDE

REM 맵 이름
SET ENGINE_PATH=D:\UE_5.5\Engine\Binaries\Win64
SET MAP_NAME=/Game/Portfolio/Menu/ServerSession

REM 빌드 끝났으면 최신화된 서버 실행
"%ENGINE_PATH%\UnrealEditor-Cmd.exe" "%PROJECT_PATH%" "%MAP_NAME%" -server -log -nosteam

pause