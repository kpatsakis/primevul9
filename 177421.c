static int __init memory_failure_init(void)
{
	struct memory_failure_cpu *mf_cpu;
	int cpu;

	for_each_possible_cpu(cpu) {
		mf_cpu = &per_cpu(memory_failure_cpu, cpu);
		spin_lock_init(&mf_cpu->lock);
		INIT_KFIFO(mf_cpu->fifo);
		INIT_WORK(&mf_cpu->work, memory_failure_work_func);
	}

	return 0;
}