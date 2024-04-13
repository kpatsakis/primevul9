static __init int rb_test(void *arg)
{
	struct rb_test_data *data = arg;

	while (!kthread_should_stop()) {
		rb_write_something(data, false);
		data->cnt++;

		set_current_state(TASK_INTERRUPTIBLE);
		/* Now sleep between a min of 100-300us and a max of 1ms */
		usleep_range(((data->cnt % 3) + 1) * 100, 1000);
	}

	return 0;
}