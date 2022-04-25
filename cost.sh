DIRS="benchmark"
for i in $DIRS; do
  list=$(ls $i);
        for file in $list; do
        f_slice=$(echo $file|cut -d '.' -f 1); # the name without extension
  ./code_constraint $i/$file optimal ../../ru/${f_slice}_ru.txt ../../final_cost/cost.txt
  done;
done;

for i in $DIRS; do
  list=$(ls $i);
        for file in $list; do
        f_slice=$(echo $file|cut -d '.' -f 1); # the name without extension
  ./code_constraint $i/$file optimal ../../ru_ori/${f_slice}_ru_ori.txt ../../final_cost_ori/cost_ori.txt ori
  done;
done;