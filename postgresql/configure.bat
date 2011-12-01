@ECHO OFF
REM *** CONFIGURATION ***
set WORKSHOP=D:\dev\_github\workshop
set MODULE=postgresql
set PGSQL=C:\Program Files (x86)\PostgreSQL\9.0
REM ********************************
set PROJECT=workshop-%MODULE%
set SRC=%WORKSHOP%\%MODULE%
set BUILD=%WORKSHOP%\%MODULE%\build
set BUILD_TYPE=Debug
set G="Visual Studio 11"
set PostgreSQL_ROOT=%PGSQL%
set PATH=%PGSQL%\bin;%PATH%
IF NOT EXIST %BUILD% mkdir %BUILD%
cd %BUILD%
cmake -G %G% %SRC%
CALL fix_vs11_sln.bat %PROJECT%.sln
cd %SRC%