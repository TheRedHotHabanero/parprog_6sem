for (( n = 1; n < 17; n++))
do
    echo "n: $n"
    for (( i = 0; i < 50; i++))
    do
        ./main $n 1e-6
    done
done > ./statistic2.txt