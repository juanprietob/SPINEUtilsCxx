
imageDir=$1
outDir=$2
idfile=$3

if [[ -n $5 ]]; then
	sessionId=$5
fi

USER=jprieto
PASSORD=UncK-9Doggy
PROJECT=NPMSC_MGHB
subject=TEST_MGHB2

for dcm in $(find ${imageDir} -name *.dcm)
do 
        #grep -l $patientID $files | while read xml; do
                #for dcm in $(find ${xml%/*}  -name *.dcm); do

    session=$(./SPINEAnonymize -dcm ${dcm} -outdcm ${outDir}/${dcm#${imageDir}*} -idfile ${idfile})

    u=${outDir}/${dcm#${imageDir}*}
    erg=`curl -H 'Content-Type: application/dicom' \
        -X POST -u $USER:$PASSWORD \
        "https://xnat.utahdcc.org/DCCxnat/data/services/import?inbody=true&PROJECT_ID=$PROJECT&SUBJECT_ID=$subject&EXPT_LABEL=$session" \
        --data-binary @$u | tr -d [:cntrl:]`
    #${erg}

    args="-X POST -u $USER:$PASSWORD"
    url=https://xnat.utahdcc.org/DCCxnat/${erg}?action=build
    echo "CALLING URL: $url ($args)"
    curl $args $url

    url=https://xnat.utahdcc.org/DCCxnat/${erg}?action=commit
    curl $args $url

    args="-X POST -u $USER:$PASSWORD"
    url="https://xnat.utahdcc.org/DCCxnat/data/services/archive?overwrite=delete&src=${erg}&dest=/archive/projects/$PROJECT/subjects/$subject"
    curl $args $url


                #done;
        #done
done

#echo curl -H 'Content-Type: application/dicom' -X POST -u jprieto:UncK-9Doggy "https://xnat.utahdcc.org/DCCxnat/data/services/import?inbody=true&PROJECT_ID=NPMSC_MGHB&SUBJECT_ID=TEST_MGHB1&EXPT_LABEL=testSession" --data-binary @/Users/prieto/NetBeansProjects/SPINEUtilsCxx/bin/APPLI/test/1.2.124.113532.132.183.36.32.20080606.200205.20832599/1.2.124.113532.132.183.169.47.20080607.60654.5700/1.2.124.113532.132.183.169.47.20080607.60655.5710.dcm
