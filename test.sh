binary="test test-coalesce"

make clean; make

score=0

for file in $binary; do
    if [[ ! -f "$file" ]]; then
        echo "FAIL: $file not made"
        echo "SCORE: $score/9"
        exit 0 
    fi
done


for (( i = 0; i < 3; i++ )); do
    echo "TEST: ./test $i"
    ./test $i > /dev/null
    if [ "$?" -eq 1 ]; then
    ((score+=1))
    else
    echo "  --FAIL!"
    fi
done

for (( i = 0; i < 4; i++ )); do
    echo "TEST: ./test-coalesce $i"
    ./test-coalesce $i > /dev/null
    if [ "$?" -eq 1 ]; then
    ((score+=1))
    else
    echo "  --FAIL!"
    fi
done



echo "TEST: plotted images are present"
if [ -f "plot-fail_rate.png" ] && [ -f "plot-throughput.png" ]; then
    ((score+=1))
else
    echo "  --FAIL!"
fi


echo "TEST: questions.txt is non-empty and contains questions (1) through (N)"
N=3
if [ -f questions.txt ] && [ "$(wc -c < questions.txt)" -gt 200 ]; then
    passed=true
    for ((i=1; i<=N; i++)); do
        if ! grep -q "^($i)" questions.txt; then
            echo "  --FAIL: questions.txt missing ($i)"
            passed=false
        fi
    done
    if $passed; then
        ((score+=1))
    fi
else
    echo "  --FAIL!"
fi

echo "SCORE: $score/9"
