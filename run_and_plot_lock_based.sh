
threads="1 2 4 8 16 32"
bench_duration="1"
algorithm="lock_based lock_free"
contention="low high"
repeat="1 2 3 4 5"

for cont in $contention
do
	if [ $cont == "low" ]
	then	
		work=2000
	else
		work=0
	fi
	for algo in $algorithm
	do	
		for t in $threads
		do
			for x in $repeat
			do
				if [ $algo == "lock_based" ]
				then
				output="$(./test_lock_based $t $bench_duration $work | grep THROUGHPUT)"
				else
				output="$(./test_lock_free $t $bench_duration $work | grep THROUGHPUT)"
				fi
				echo $cont" "$algo" "$t" "$output
			done
		done
	done
done
