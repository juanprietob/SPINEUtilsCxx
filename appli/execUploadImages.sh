#SERVER="170.223.221.92"
#SERVER="170.223.221.66"
SERVER="localhost"

COOKIE="Fe26.2**762c3d51863bdcfd9cedde95caea195ced7d5cdf04c788a87e045daea9475bba*Al7IJFrQ8NX46hy7PFppbA*SVHO4jXeCEERCUpNYU65u5uia4nmguOtZ9mjZAs0rmsL6FBOMMuGXn5062-yZsXZhet4v5EFA4wStgdWO6F7XSqBD_vjo2VAyxZ8P6H_8TrrK8JWS3I-GkDKRYahoLVB0x7t6QroktsVfR9iACbEAYj2JieXUBNTVOb4Ac80jC77Q-daiu4GiUjVIxNa12pmdVar5DIsZr9O9F29PNLXBm8i_e2L-KwNewK9Gf34Me4**adef3742d425cbd89e6a966778245770b184831f44d17948a71beb51c0a5ebee*oUtcp2ujpfPYZqU-WxtOz3qinma12382wRaIn6p4dOg"


EMAIL="juanprietob@gmail.com"

imagesDir=$1


for file in $(ls ${imagesDir}/*.nii.gz)
do 
	imageName=$(basename ${file})
	name=${imageName%.nii.gz}
	imageType=${name##*_}
	name=${name%_${imageType}}
	
	
	echo curl https://$SERVER:8081/image \
   		-H "Cookie: session=$COOKIE" \
   		-X "POST" \
		-F "file=@$file" \
		-F "name=$name" \
   		-F "type=$imageType" \
   		-F "userEmail=$EMAIL" \
		-k
	
done;


