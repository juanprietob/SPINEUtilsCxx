directory=$1

for dirs in $(ls -d ${directory}/*/)
do 
	for files in $(find ${dirs} -name I.001)
                do ./ImageSeriesReadWrite -f $files -d $dirs >> output.txt
	done
done


