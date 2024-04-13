static __init int ftrace_test_event_filter(void)
{
	int i;

	printk(KERN_INFO "Testing ftrace filter: ");

	for (i = 0; i < DATA_CNT; i++) {
		struct event_filter *filter = NULL;
		struct test_filter_data_t *d = &test_filter_data[i];
		int err;

		err = create_filter(NULL, &event_ftrace_test_filter,
				    d->filter, false, &filter);
		if (err) {
			printk(KERN_INFO
			       "Failed to get filter for '%s', err %d\n",
			       d->filter, err);
			__free_filter(filter);
			break;
		}

		/* Needed to dereference filter->prog */
		mutex_lock(&event_mutex);
		/*
		 * The preemption disabling is not really needed for self
		 * tests, but the rcu dereference will complain without it.
		 */
		preempt_disable();
		if (*d->not_visited)
			update_pred_fn(filter, d->not_visited);

		test_pred_visited = 0;
		err = filter_match_preds(filter, &d->rec);
		preempt_enable();

		mutex_unlock(&event_mutex);

		__free_filter(filter);

		if (test_pred_visited) {
			printk(KERN_INFO
			       "Failed, unwanted pred visited for filter %s\n",
			       d->filter);
			break;
		}

		if (err != d->match) {
			printk(KERN_INFO
			       "Failed to match filter '%s', expected %d\n",
			       d->filter, d->match);
			break;
		}
	}

	if (i == DATA_CNT)
		printk(KERN_CONT "OK\n");

	return 0;
}