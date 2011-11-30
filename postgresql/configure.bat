@ECHO OFF
REM *** YUR CONFIGURATION ***
set SRC=G:\dev\mloskot\_git\workshop\postgresql
set BUILD=G:\dev\mloskot\_git\workshop\postgresql\build
set BUILD_TYPE=Debug
set PGSQL=D:\Program Files (x86)\PostgreSQL\9.0
REM ********************************
set PostgreSQL_ROOT=%PGSQL%
set PATH=%PGSQL%\bin;%PATH%
IF NOT EXIST %BUILD% mkdir %BUILD%
cd %BUILD%
cmake %SRC%
cd %SRC%