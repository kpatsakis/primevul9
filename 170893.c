static __init int test_ringbuffer(void)
{
	struct task_struct *rb_hammer;
	struct ring_buffer *buffer;
	int cpu;
	int ret = 0;

	pr_info("Running ring buffer tests...\n");

	buffer = ring_buffer_alloc(RB_TEST_BUFFER_SIZE, RB_FL_OVERWRITE);
	if (WARN_ON(!buffer))
		return 0;

	/* Disable buffer so that threads can't write to it yet */
	ring_buffer_record_off(buffer);

	for_each_online_cpu(cpu) {
		rb_data[cpu].buffer = buffer;
		rb_data[cpu].cpu = cpu;
		rb_data[cpu].cnt = cpu;
		rb_threads[cpu] = kthread_create(rb_test, &rb_data[cpu],
						 "rbtester/%d", cpu);
		if (WARN_ON(!rb_threads[cpu])) {
			pr_cont("FAILED\n");
			ret = -1;
			goto out_free;
		}

		kthread_bind(rb_threads[cpu], cpu);
 		wake_up_process(rb_threads[cpu]);
	}

	/* Now create the rb hammer! */
	rb_hammer = kthread_run(rb_hammer_test, NULL, "rbhammer");
	if (WARN_ON(!rb_hammer)) {
		pr_cont("FAILED\n");
		ret = -1;
		goto out_free;
	}

	ring_buffer_record_on(buffer);
	/*
	 * Show buffer is enabled before setting rb_test_started.
	 * Yes there's a small race window where events could be
	 * dropped and the thread wont catch it. But when a ring
	 * buffer gets enabled, there will always be some kind of
	 * delay before other CPUs see it. Thus, we don't care about
	 * those dropped events. We care about events dropped after
	 * the threads see that the buffer is active.
	 */
	smp_wmb();
	rb_test_started = true;

	set_current_state(TASK_INTERRUPTIBLE);
	/* Just run for 10 seconds */;
	schedule_timeout(10 * HZ);

	kthread_stop(rb_hammer);

 out_free:
	for_each_online_cpu(cpu) {
		if (!rb_threads[cpu])
			break;
		kthread_stop(rb_threads[cpu]);
	}
	if (ret) {
		ring_buffer_free(buffer);
		return ret;
	}

	/* Report! */
	pr_info("finished\n");
	for_each_online_cpu(cpu) {
		struct ring_buffer_event *event;
		struct rb_test_data *data = &rb_data[cpu];
		struct rb_item *item;
		unsigned long total_events;
		unsigned long total_dropped;
		unsigned long total_written;
		unsigned long total_alloc;
		unsigned long total_read = 0;
		unsigned long total_size = 0;
		unsigned long total_len = 0;
		unsigned long total_lost = 0;
		unsigned long lost;
		int big_event_size;
		int small_event_size;

		ret = -1;

		total_events = data->events + data->events_nested;
		total_written = data->bytes_written + data->bytes_written_nested;
		total_alloc = data->bytes_alloc + data->bytes_alloc_nested;
		total_dropped = data->bytes_dropped + data->bytes_dropped_nested;

		big_event_size = data->max_size + data->max_size_nested;
		small_event_size = data->min_size + data->min_size_nested;

		pr_info("CPU %d:\n", cpu);
		pr_info("              events:    %ld\n", total_events);
		pr_info("       dropped bytes:    %ld\n", total_dropped);
		pr_info("       alloced bytes:    %ld\n", total_alloc);
		pr_info("       written bytes:    %ld\n", total_written);
		pr_info("       biggest event:    %d\n", big_event_size);
		pr_info("      smallest event:    %d\n", small_event_size);

		if (RB_WARN_ON(buffer, total_dropped))
			break;

		ret = 0;

		while ((event = ring_buffer_consume(buffer, cpu, NULL, &lost))) {
			total_lost += lost;
			item = ring_buffer_event_data(event);
			total_len += ring_buffer_event_length(event);
			total_size += item->size + sizeof(struct rb_item);
			if (memcmp(&item->str[0], rb_string, item->size) != 0) {
				pr_info("FAILED!\n");
				pr_info("buffer had: %.*s\n", item->size, item->str);
				pr_info("expected:   %.*s\n", item->size, rb_string);
				RB_WARN_ON(buffer, 1);
				ret = -1;
				break;
			}
			total_read++;
		}
		if (ret)
			break;

		ret = -1;

		pr_info("         read events:   %ld\n", total_read);
		pr_info("         lost events:   %ld\n", total_lost);
		pr_info("        total events:   %ld\n", total_lost + total_read);
		pr_info("  recorded len bytes:   %ld\n", total_len);
		pr_info(" recorded size bytes:   %ld\n", total_size);
		if (total_lost)
			pr_info(" With dropped events, record len and size may not match\n"
				" alloced and written from above\n");
		if (!total_lost) {
			if (RB_WARN_ON(buffer, total_len != total_alloc ||
				       total_size != total_written))
				break;
		}
		if (RB_WARN_ON(buffer, total_lost + total_read != total_events))
			break;

		ret = 0;
	}
	if (!ret)
		pr_info("Ring buffer PASSED!\n");

	ring_buffer_free(buffer);
	return 0;
}