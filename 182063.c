static void __spin_time_accum(u64 delta, u32 *array)
{
	unsigned index;

	index = ilog2(delta);
	check_zero();

	if (index < HISTO_BUCKETS)
		array[index]++;
	else
		array[HISTO_BUCKETS]++;
}