
USER=jprieto
PASSORD=UncK-9Doggy
PROJECT=NPMSC_MGHB
subject=TEST_MGHB2

for dcm in $(find ${imageDir} -name *.dcm)
do

    session=$(./SPINEAnonymizeGetSessionId ${dcm})

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

done
