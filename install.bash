pico=/media/$USER/RPI-RP2

if ! [[ -d "$pico" ]]
then
	echo "$pico does not exist."
	exit 1
fi

if ! [[ -d build/ ]]
then
	echo "Compile project first."
	exit 1
fi

cp build/main.uf2 $pico
echo "Installed main.uf2 to $pico."
