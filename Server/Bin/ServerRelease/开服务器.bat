@echo off 
echo 注意：在服务器启动过程中，不要按键继续，等等就可以了，不会很久的。
echo 启动中心服务器
START "%cd%\CenterServer.exe" "CenterServer.exe"
TIMEOUT /T 2 
echo 启动数据库服务器
START "%cd%\DBServer.exe" "DBServer.exe"
TIMEOUT /T 2 
echo 启动登陆认证服务器
START "%cd%\VerifyServer.exe" "VerifyServer.exe"
TIMEOUT /T 2 
ECHO 启动数据服务器 
START "%cd%\DataServer" "DataServer.exe"
TIMEOUT /T 1 
echo 启动登陆服务器
START "%cd%\loginServer.exe" "loginServer.exe"
TIMEOUT /T 1 
ECHO 启动游戏服务器 
START "%cd%\MJServer.exe" "MJServer.exe"
TIMEOUT /T 1 
ECHO 启动log服务器 
START "%cd%\LogServer.exe" "LogServer.exe"
TIMEOUT /T 1 
ECHO 启动网关服务器 
START "%cd%\GateServer.exe" "GateServer.exe"
::TIMEOUT /T 3 > nul
ECHO 服务器启动完毕,命令行即将退出
TIMEOUT /T 3

