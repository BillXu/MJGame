@echo off 
echo ע�⣺�ڷ��������������У���Ҫ�����������ȵȾͿ����ˣ�����ܾõġ�
echo �������ķ�����
START "%cd%\CenterServer.exe" "CenterServer.exe"
TIMEOUT /T 5 
echo �������ݿ������
START "%cd%\DBServer.exe" "DBServer.exe"
TIMEOUT /T 3 
ECHO �������ݷ����� 
START "%cd%\DataServer" "DataServer.exe"
TIMEOUT /T 1 
echo ������½������
START "%cd%\loginServer.exe" "loginServer.exe"
TIMEOUT /T 1 
ECHO �������ط����� 
START "%cd%\GateServer.exe" "GateServer.exe"
::TIMEOUT /T 3 > nul
ECHO �������������,�����м����˳�
TIMEOUT /T 3

