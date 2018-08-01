#!/bin/bash
for f in pic00*.jpg ; do convert -resize 1024x2048! $f $f; done
for f in pic00*.jpg ; do convert -rotate -90 $f $f; done

N=0
Q=1
F=0
for f in "$FILES"; do
	N=$((N+1))
	Q=$((Q+1))
	
	if [ $N -lt 10 ]
	then
		name="pic0000$N.jpg"
	elif [ $N -lt 100 ]
	then
		name="pic000$N.jpg"
	elif [ $N -lt 1000 ]
	then
		name="pic00$N.jpg"
	fi

	if [ $Q -gt 203 ]
	then
		$Q = 1
	fi

	if [ $Q -lt 10 ]
	then
		name2="pic0000$Qjpg"
	elif [ $Q -lt 100 ]
	then
		name2="pic000$Q.jpg"
	elif [ $NQ-lt 1000 ]
	then
		name2="pic00$Q.jpg"
	fi

	morph $name $name2 32 proesmans frame

	for i in {1..32}
	do
		F=$((F+1))
		if [ $F -lt 10 ]
		then
			frame="frame000000$F.tga"
		elif [ $F -lt 100 ]
		then
			frame="frame00000$F.tga"
		elif [ $F -lt 1000 ]
		then
			frame="frame0000$F.tga"
		elif [ $F -lt 10000 ]
		then
			frame="frame000$F.tga"
		elif [ $F -lt 100000 ]
		then
			frame="frame00$F.tga"
		fi

		mv "frame-morph-$i.png" "frames/$frame"
	done
done