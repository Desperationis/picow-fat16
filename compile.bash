if ! [[ -d build ]] 
then
	mkdir build
fi

cd build
rm -rf *
if cmake ..
then
	make
fi

if [[ -e compile_commands.json ]]
then
	mv compile_commands.json ..
fi
