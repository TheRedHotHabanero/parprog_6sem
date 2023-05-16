for (( n = 1; n < 9; n++))
do
  echo "n: $n"
  echo "X: 10000"
  echo "T: 1"
  for (( i = 0; i < 10; i++))
  do
    mpirun -np $n ./x_template
  done
done > ./statistic.dat