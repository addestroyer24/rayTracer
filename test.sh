./build.sh
if [ $? != 0 ] ; then
	exit 1
fi

cd build
BUILD_LIST=(blue_sphere cornell_box spheres)

for file in ${BUILD_LIST[@]} ;
do
	echo "Rendering $file..."
	./tracer scenes/$file.obj $file.png 
	if [ $? != 0 ] ; then
		exit 1
	fi
	if [ "$1" == "show" ] ; then
		eog $file.png
	fi
done
