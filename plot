make clean; make

mkdir data
rm -f data/t.out
rm -f data/t2.out

for (( i = 4; i < 8; i++ )); do
    for (( j = 0; j < 10; j++ )); do
        echo "BENCH: ./test $i"
        ./test $i >> data/t.out
    done
done

for (( i = 4; i < 8; i++ )); do
    for (( j = 0; j < 10; j++ )); do
        echo "BENCH: ./test-coalesce $i"
        ./test-coalesce $i >> data/t2.out
    done
done
