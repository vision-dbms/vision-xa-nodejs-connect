#!/usr/bin/tcsh

#-------------------------------
# Variables
#-------------------------------

set TestDir = `pwd` 
set LogFile = $TestDir/LOGS/current

#-------------------------------
# Input Argument Check
#-------------------------------

if ($#argv != 2) then
   echo "Usage: ${0} <Type> <Socket>"
   echo "Type: File | Raw"
   echo "Socket: SocketFilePath | SocketAddress"
   exit
else if (${1} != "File" && ${1} != "Raw") then
   echo "Invalid Type in Usage"
   exit
else if (${1} == "File") then
   if (! -e ${2}) then
	echo "Invalid File specified"
	exit
   endif
endif

# Check for Tools Env variable
if (! $?Tools) then
   echo "Tools Environment variable needs to be set"
   exit
endif

set Type = ${1}
set Socket = ${2}

if ($Type == "File") then
   set serverArg = "-serverFile=$Socket"
else
   set serverArg = "-server=$Socket"
endif 

#---------------------------------
# Test
#---------------------------------

set ConnectionResult = `$Tools/vpooladmin $serverArg`

echo "-------------------------------------------------" >> $LogFile
echo "Pool     : $Socket" >> $LogFile
echo "TestDate : `date`" >> $LogFile
echo -n "Connection Test         :" >> $LogFile
echo -n "Connection Test         :"

echo $ConnectionResult | grep Error >> /dev/null
if ($status == 0) then
   echo  "FAILED"
   echo  "FAILED" >> $LogFile
   echo  $ConnectionResult >> $LogFile
else 
   echo  "PASSED"
   echo  "PASSED" >> $LogFile
endif
