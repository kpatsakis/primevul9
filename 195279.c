static void release_ds_buffer(int cpu)
{
	per_cpu(cpu_hw_events, cpu).ds = NULL;
}