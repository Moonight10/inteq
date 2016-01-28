#z3的安装目录
#/home/haosun/intchecker/z3/z3-dest/bin/z3 -smt2 

#!/bin/bash
function ergodic(){
for file in `ls $1`
do
	#local name=$file[[ $str =~ "this" ]]
	if [[ $file =~ "~" ]]
	then
		:	
	else
		echo $file
		/home/haosun/intchecker/z3/z3-dest/bin/z3 -smt2 $1"/"$file
	fi
			#if [ -d $1"/"$file] 
		#如果 file存在且是一个目录则为真
		#then
		#      ergodic $1"/"$file
		#else
		 #     local path=$1"/"$file #得到文件的完整的目录
		 #     local name=$file       #得到文件的名字
		  #    echo "good"
			  #做自己的工作.
		#fi
done
}
z3pathdir="/tmp/z3paths"
ergodic $z3pathdir 

