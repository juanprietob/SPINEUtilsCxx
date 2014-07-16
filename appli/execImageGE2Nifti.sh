

patientsDir=$1
patientId=$2

mkdir -p temp

for patientDirs in $(ls -d ${patientsDir}/*${patientId}*);
do
	cp -r ${patientDirs} ./temp
	patientDir=$(basename ${patientDirs})


	for files in $(find ./temp/${patientDir} -name *.gz);
	do
		gzip -f -d ${files}
	done

	sessionId=${patientDir##*_}

	echo 'sessionId='${sessionId}
	echo 'patientDir='${patientDir}


	for imageTypes in $(ls -d ./temp/${patientDir}/*/)
	do 
		
		for files in $(find ./temp/${patientDir}/${imageType} -name I.001)
    	do 
    		tempImageType=${files%%I.001}
    		imageType=$(basename ${tempImageType})
        	./ImageSeriesReadWrite -f $files -o ./temp/${patientId}_${sessionId}_${imageType}.nii.gz
		done
        
	done

	rm -rf ./temp/${patientDir}
done


