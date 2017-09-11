#!/bin/bash
ARG="$1"
# getYesNo
# Asks the user the given question and returns a 1 for yes and a 0 for no
function getYesNo ()
{	


	output=2
	message=$1
	valid=false
	until [ $valid == true ]
	do
	
	echo -n "$message (y/n): "
	read choice
	choice=$(echo "${choice^^}" )
		case $choice in
			"Y"|"YES")
				output=1
				valid=true
				;;

			"N"|"NO")
				output=0
				valid=true
				;;
		esac
	done

	return $output
}

# just added

#CMAKE_BUILD_TYPE=Release
CMAKE_BUILD_TYPE=Debug

BUILD_DIR=bin

CMAKE_CMD="cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE ../.."


# Create and move to build dir
mkdir -p $BUILD_DIR/$CMAKE_BUILD_TYPE
cd $BUILD_DIR/$CMAKE_BUILD_TYPE


$CMAKE_CMD

make
RESULT=$?
if [ "$ARG" != "-n" ] && [ "$RESULT" == "0" ]
then
	getYesNo "Would you like to run the build?"
	answer=$?
	if [ "$answer" == 1 ]
	then
		echo "running " 
		./gpstest
	
		else
	
		echo "done"
	fi
fi

