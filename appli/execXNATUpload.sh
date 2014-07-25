
imageDir=$1
USER=jprieto
PASSWORD=UncK-9Doggy
PROJECT=T01
SUBJECT=testSub
SESSION=${SUBJECT}_MR1


for dcm in $(find ${imageDir} -name *.dcm)
do


    u=${outDir}/${dcm#${imageDir}*}
    erg=`curl -H 'Content-Type: application/dicom' \
        -X POST -u $USER:$PASSWORD \
        "https://xnat.utahdcc.org/DCCxnat/data/services/import?inbody=true&PROJECT_ID=$PROJECT&SUBJECT_ID=$SUBJECT&EXPT_LABEL=$SESSION" \
        --data-binary @$dcm | tr -d [:cntrl:]`


done
