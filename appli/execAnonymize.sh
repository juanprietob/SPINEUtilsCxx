
imageDir=$1
outDir=$2
idfile=$3

for dcm in $(find ${imageDir} -name *.dcm)
do 
        #grep -l $patientID $files | while read xml; do
                #for dcm in $(find ${xml%/*}  -name *.dcm); do

    ./SPINEAnonymize -dcm ${dcm} -outdcm ${outDir}/${dcm#${imageDir}*} -idfile ${idfile}
done

#echo curl -H 'Content-Type: application/dicom' -X POST -u jprieto:UncK-9Doggy "https://xnat.utahdcc.org/DCCxnat/data/services/import?inbody=true&PROJECT_ID=NPMSC_MGHB&SUBJECT_ID=TEST_MGHB1&EXPT_LABEL=testSession" --data-binary @/Users/prieto/NetBeansProjects/SPINEUtilsCxx/bin/APPLI/test/1.2.124.113532.132.183.36.32.20080606.200205.20832599/1.2.124.113532.132.183.169.47.20080607.60654.5700/1.2.124.113532.132.183.169.47.20080607.60655.5710.dcm
