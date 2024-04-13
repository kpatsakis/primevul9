static __init void rb_ipi(void *ignore)
{
	struct rb_test_data *data;
	int cpu = smp_processor_id();

	data = &rb_data[cpu];
	rb_write_something(data, true);
}