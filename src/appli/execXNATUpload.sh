imageDir=$1
PROJECT=NPMSC_MGHB
JSESSIONID=2295EE7706630FB24DB84955D8B27EBA

for patientDir in $(ls -d $imageDir/*/);
do
        SUBJECT=$(basename ${patientDir##$imageDir})
        for session in $(ls -d $patientDir/*/)
        do
                SESSION=$(basename ${session##$patientDir})

                find $session -name "*.dcm" -exec curl  -H 'Content-Type: application/dicom' -X POST --cookie JSESSIONID=$JSESSIONID "https://xnat.utahdcc.org/DCCxnat/data/services/import?inbody=true&PROJECT_ID=$PROJECT&SUBJECT_ID=$SUBJECT&EXPT_LABEL=$SESSION" --data-binary @'{}' \;
        done
done
