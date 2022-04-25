DIRS="benchmark"
for i in $DIRS; do
  list=$(ls $i);
        for file in $list; do
        f_slice=$(echo $file|cut -d '.' -f 1); # the name without extension
	./code_constraint $i/$file constraint ../../flow/${f_slice}_mid.txt 
  ./code_constraint $i/$file constraint ../../flow_ori/${f_slice}_mid_ori.txt ori 
	done;
done;
#for i in $DIRS; do
 # list=$(ls $i);
 #       for file in $list; do
  #      f_slice=$(echo $file|cut -d '.' -f 1); # the name without extension
	#./code_constraint $i/$file constraint ../../flow_ori/${f_slice}_mid_ori.txt ori 
	#done;
#done;

DIRS2="../../circuit/circuit/benchmark"
cd ../../matlab/tomlab
source ~/.profile
#pwd
#matlab  -r startup.m ;
#quit()
for i in $DIRS2; do
  list=$(ls $i);
        for file in $list; do
        f_slice=$(echo $file|cut -d '.' -f 1); # the name without extension
        matlab -r "calculate_ru('../../flow/${f_slice}_mid.txt','../../ru/${f_slice}_ru.txt');exit"
        matlab -r "calculate_ru('../../flow_ori/${f_slice}_mid_ori.txt','../../ru_ori/${f_slice}_ru_ori.txt');exit"
     
  done;
done;
cd ../../circuit/circuit
#matlab -r "calculate_ru('../../flow/i10_mid.txt','../../u_r_bigsb.txt');exit"
for i in $DIRS; do
  list=$(ls $i);
        for file in $list; do
        f_slice=$(echo $file|cut -d '.' -f 1); # the name without extension
  ./code_constraint $i/$file optimal ../../ru/${f_slice}_ru.txt ../../final_cost/cost.txt
  ./code_constraint $i/$file optimal ../../ru_ori/${f_slice}_ru_ori.txt ../../final_cost_ori/cost_ori.txt ori

  done;
done;

#for i in $DIRS; do
#  list=$(ls $i);
#        for file in $list; do
#        f_slice=$(echo $file|cut -d '.' -f 1); # the name without extension
#  ./code_constraint $i/$file optimal ../../ru_ori/${f_slice}_ru_ori ../../final_cost_ori/cost_ori.txt ori
#  done;
#done;





