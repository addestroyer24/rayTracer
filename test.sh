DEFAULT_BUILD_LIST=(blue_sphere bunny-scene cornell_box happy-scene spheres)

cd build

SHOW=0
I=0

for arg in $@ ;
do
	if [[ $arg == "-s" ]] ;
	then
		SHOW=1
	else
		BUILD_LIST[$I]=$arg
		I=$I+1
	fi
done

if [[ "$BUILD_LIST" == "" ]] ;
then
	BUILD_LIST=${DEFAULT_BUILD_LIST[@]}
fi

for file in ${BUILD_LIST[@]} ;
do
	echo -e "\nRendering $file..."
	./tracer scenes/$file.obj $file.png
	if [[ $SHOW == 1 && $? == 0 ]] ; 
	then
		eog $file.png &
	fi
done

