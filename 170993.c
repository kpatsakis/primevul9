static __init int rb_hammer_test(void *arg)
{
	while (!kthread_should_stop()) {

		/* Send an IPI to all cpus to write data! */
		smp_call_function(rb_ipi, NULL, 1);
		/* No sleep, but for non preempt, let others run */
		schedule();
	}

	return 0;
}