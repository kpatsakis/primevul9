struct ring_buffer *__ring_buffer_alloc(unsigned long size, unsigned flags,
					struct lock_class_key *key)
{
	struct ring_buffer *buffer;
	long nr_pages;
	int bsize;
	int cpu;

	/* keep it in its own cache line */
	buffer = kzalloc(ALIGN(sizeof(*buffer), cache_line_size()),
			 GFP_KERNEL);
	if (!buffer)
		return NULL;

	if (!alloc_cpumask_var(&buffer->cpumask, GFP_KERNEL))
		goto fail_free_buffer;

	nr_pages = DIV_ROUND_UP(size, BUF_PAGE_SIZE);
	buffer->flags = flags;
	buffer->clock = trace_clock_local;
	buffer->reader_lock_key = key;

	init_irq_work(&buffer->irq_work.work, rb_wake_up_waiters);
	init_waitqueue_head(&buffer->irq_work.waiters);

	/* need at least two pages */
	if (nr_pages < 2)
		nr_pages = 2;

	/*
	 * In case of non-hotplug cpu, if the ring-buffer is allocated
	 * in early initcall, it will not be notified of secondary cpus.
	 * In that off case, we need to allocate for all possible cpus.
	 */
#ifdef CONFIG_HOTPLUG_CPU
	cpu_notifier_register_begin();
	cpumask_copy(buffer->cpumask, cpu_online_mask);
#else
	cpumask_copy(buffer->cpumask, cpu_possible_mask);
#endif
	buffer->cpus = nr_cpu_ids;

	bsize = sizeof(void *) * nr_cpu_ids;
	buffer->buffers = kzalloc(ALIGN(bsize, cache_line_size()),
				  GFP_KERNEL);
	if (!buffer->buffers)
		goto fail_free_cpumask;

	for_each_buffer_cpu(buffer, cpu) {
		buffer->buffers[cpu] =
			rb_allocate_cpu_buffer(buffer, nr_pages, cpu);
		if (!buffer->buffers[cpu])
			goto fail_free_buffers;
	}

#ifdef CONFIG_HOTPLUG_CPU
	buffer->cpu_notify.notifier_call = rb_cpu_notify;
	buffer->cpu_notify.priority = 0;
	__register_cpu_notifier(&buffer->cpu_notify);
	cpu_notifier_register_done();
#endif

	mutex_init(&buffer->mutex);

	return buffer;

 fail_free_buffers:
	for_each_buffer_cpu(buffer, cpu) {
		if (buffer->buffers[cpu])
			rb_free_cpu_buffer(buffer->buffers[cpu]);
	}
	kfree(buffer->buffers);

 fail_free_cpumask:
	free_cpumask_var(buffer->cpumask);
#ifdef CONFIG_HOTPLUG_CPU
	cpu_notifier_register_done();
#endif

 fail_free_buffer:
	kfree(buffer);
	return NULL;
}