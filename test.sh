BUILD_LIST=(blue_sphere cornell_box spheres)

cd build

for file in ${BUILD_LIST[@]} ;
do
	echo -e "\nRendering $file..."
	./tracer scenes/$file.obj $file.png 
	if [ "$1" == "show" ] ; then
		eog $file.png &
	fi
done

