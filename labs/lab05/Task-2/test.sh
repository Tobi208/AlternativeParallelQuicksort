exe=$1
N=$2

for (( i=0; i < $N; i++ )); do
    time ./$exe
done