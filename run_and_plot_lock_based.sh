threads="1 2 4 8 16"
repeat="5"
bench_duration="5"

echo "Lock based"
for t in $threads
do
	./test_lock_based $t $bench_duration | grep THROUGHPUT
done
echo "Lock free"
for t in $threads
do
	./test_lock_free $t $bench_duration | grep THROUGHPUT
done
