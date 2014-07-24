
USER=jprieto
PASSWORD=UncK-9Doggy
PROJECT=NPMSC_MGHB
SUBJECT=TEST_MGHB2
SESSION=${SUBJECT}_MR1


for dcm in $(find ${imageDir} -name *.dcm)
do


    u=${outDir}/${dcm#${imageDir}*}
    erg=`curl -H 'Content-Type: application/dicom' \
        -X POST -u $USER:$PASSWORD \
        "https://xnat.utahdcc.org/DCCxnat/data/services/import?inbody=true&PROJECT_ID=$PROJECT&SUBJECT_ID=$SUBJECT&EXPT_LABEL=$session" \
        --data-binary @$dcm | tr -d [:cntrl:]`


done
