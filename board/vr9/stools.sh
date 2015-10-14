#!/bin/bash
#shell script to generate secure boot image
#revision history
#12/12/2011 Wu Qi Ming initial version


SECURE_HEADER_MAGIC=0x04750736
CUSTOMER_MAGIC=0x25269473
CUSTOMER_XOR=0x45057602


Usage(){
  echo "Usage: stools.sh [option]...[file]..."
  echo "-h  --help             print help information"
  echo "-i  --in   input       input file"
  echo "-o  --out  output      output file"
  echo "-k  --key  key         cbc key"
  echo "-v  --iv   iv          cbc init vector"
  echo "-d  --ds   dsfile      digital signature file"
  echo "-m  --mod  mod         rsa mod"
  echo "-e  --exp  exp         rsa public exponential"
  echo "-a  --addr address     dycrypt target address"
  echo "-s  --shield           shield aes key, use default key in the chip"
  return
}

INPUTFILE=
OUTPUTFILE=
SHIELD=0
CBCKEY_MASKED=1212121212121212343434343434343456565656565656567878787878787878
CBCIV=00000000000000000000000000000000
DSALGO=ds_crypt.bin
DSALGO_ADDR=0xbe220050
RSAMOD=24287
RSAEXP=11
TARGET_ADDR=0xbe220000


parse_args() {
  while [ "$#" -gt "0" ]
  do
     case $1 in
	        --in | -i)
		        INPUTFILE=$2		
			;;
                --out | -o)
                        OUTPUTFILE=$2
                        ;;
                --key | -k)
                        CBCKEY=$2
                        ;;
                --iv | -v)
                        CBCIV=$2
                        ;;
	          	--ds | -d)
                        DSALGO=$2
                        ;;
                --mod | -m)
                        RSAMOD=$2
                        ;;
                --exp | -e)
                        RSAEXP=$2
                        ;;
                --addr | -a)
                        TARGET_ADDR=$2
                        ;;
				--shiled | -s)
				        SHIELD=1 
				        ;;
                --help | -h )
			Usage
			exit 1
			;;
      esac			
      shift 
  done
  return
}

packhex(){
    PACK_DATA_LEN=`echo $1 |wc -c`
    let PACK_DATA_LEN=$PACK_DATA_LEN-1
    PACK_START=1
    PACK_RESULT=
    while [ $PACK_START -le $PACK_DATA_LEN ]
    do
       PACK_TMP=`echo $1 | awk '{print substr($0, '$PACK_START', 2)}'`
       PACK_RESULT="${PACK_RESULT}\x${PACK_TMP}" 
       let PACK_START=$PACK_START+2
    done
    echo $PACK_RESULT
}

if [ $# -eq 0 ] ; then  
   Usage
   exit 1
fi 

parse_args $@

echo "input:" $INPUTFILE
echo "output:" $OUTPUTFILE
echo "cbckey:" $CBCKEY
echo "cbciv:"  $CBCIV
echo "dsalgo:" $DSALGO

openssl enc -aes-256-cbc -in $INPUTFILE -out file.enc -K $CBCKEY -iv $CBCIV -nopad
SHA_RESULT=`openssl dgst -sha1 file.enc | awk -F'= ' '{print $2}'`
echo "sha result:" $SHA_RESULT 

SHA_RESULT_LEN=`echo $SHA_RESULT |wc -c`
let SHA_RESULT_LEN=$SHA_RESULT_LEN-1
#echo $SHA_RESULT_LEN

rm -rf sig
touch sig
echo "generating signature...."
START=1
while [ $START -le $SHA_RESULT_LEN ]
do
  RSA_M=`echo $SHA_RESULT | awk '{print substr($0, '$START', 2)}'`
  RSA_M=`echo $((0x${RSA_M}))`
  RSA_CRYPTED=`perl -Mbigint -e "print $RSA_M**$RSAEXP%$RSAMOD"|xargs printf "%08x\n"`
  RSA_CRYPTED=`packhex $RSA_CRYPTED`
  echo -en "${RSA_CRYPTED}" >>sig
  let START=${START}+2
done

#creating secure header now
#secure header magic
DATA=`printf "%08x" $SECURE_HEADER_MAGIC` 
echo -en `packhex $DATA` >shdr

#customer magic
DATA=`printf "%08x" $CUSTOMER_MAGIC`
echo -en `packhex $DATA` >>shdr

#dynamic decryption code address
let DATA=$DSALGO_ADDR^${CUSTOMER_XOR}
DATA=`printf "%08x" $DATA`
echo -en `packhex $DATA` >>shdr

#digital signature address
let DIGITAL_SIGN_ADDR=`wc -c $DSALGO | awk '{print $1}'`+${DSALGO_ADDR}
let DATA=$DIGITAL_SIGN_ADDR^${CUSTOMER_XOR}
DATA=`printf "%08x" $DATA`
echo -en `packhex $DATA` >>shdr

#decrypt key
if [ $SHIELD -eq 1 ] ; then
   CBCKEY=1212121212121212343434343434343456565656565656567878787878787878
fi
echo "cbckey in shdr:" $CBCKEY
START=1
while [ $START -le 64 ]
do
  DATA=`echo $CBCKEY | awk '{print substr($0, '$START', 8)}'`
  let DATA="0x${DATA}"
  let DATA^=${CUSTOMER_XOR}
  DATA=`printf "%08x" $DATA`
  echo -en `packhex $DATA` >>shdr
  let START=$START+8
done

#init vector
START=1
while [ $START -le 32 ]
do
  IV=`echo $CBCIV | awk '{print substr($0, '$START', 8)}'` 
  let IV="0x${IV}"
  let IV^=${CUSTOMER_XOR}
  IV=`echo $IV | xargs printf "%08x"`
  echo -en `packhex $IV`>>shdr
  let START=$START+8
done

#encrypted image address
let DATA=${DSALGO_ADDR}+`wc -c $DSALGO | awk '{print $1}'`+`wc -c sig | awk '{print $1}'`
let DATA^=${CUSTOMER_XOR}
DATA=`printf "%08x" $DATA`
echo -en `packhex $DATA` >>shdr

#decrypt target address
let DATA=${TARGET_ADDR}^${CUSTOMER_XOR}
DATA=`printf "%08x" $DATA`
echo -en `packhex $DATA` >>shdr

#encrypted image length
IMAGE_LEN=`wc -c file.enc | awk '{print $1}'`
let DATA=${IMAGE_LEN}^${CUSTOMER_XOR}
DATA=`printf "%08x" $DATA`
echo -en `packhex $DATA` >>shdr

#target address
let DATA=${TARGET_ADDR}^${CUSTOMER_XOR}
DATA=`printf "%08x" $DATA`
echo -en `packhex $DATA` >>shdr

cat shdr $DSALGO sig file.enc >$OUTPUTFILE