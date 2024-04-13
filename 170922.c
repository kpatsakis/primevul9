int ring_buffer_wait(struct ring_buffer *buffer, int cpu, bool full)
{
	struct ring_buffer_per_cpu *uninitialized_var(cpu_buffer);
	DEFINE_WAIT(wait);
	struct rb_irq_work *work;
	int ret = 0;

	/*
	 * Depending on what the caller is waiting for, either any
	 * data in any cpu buffer, or a specific buffer, put the
	 * caller on the appropriate wait queue.
	 */
	if (cpu == RING_BUFFER_ALL_CPUS) {
		work = &buffer->irq_work;
		/* Full only makes sense on per cpu reads */
		full = false;
	} else {
		if (!cpumask_test_cpu(cpu, buffer->cpumask))
			return -ENODEV;
		cpu_buffer = buffer->buffers[cpu];
		work = &cpu_buffer->irq_work;
	}


	while (true) {
		if (full)
			prepare_to_wait(&work->full_waiters, &wait, TASK_INTERRUPTIBLE);
		else
			prepare_to_wait(&work->waiters, &wait, TASK_INTERRUPTIBLE);

		/*
		 * The events can happen in critical sections where
		 * checking a work queue can cause deadlocks.
		 * After adding a task to the queue, this flag is set
		 * only to notify events to try to wake up the queue
		 * using irq_work.
		 *
		 * We don't clear it even if the buffer is no longer
		 * empty. The flag only causes the next event to run
		 * irq_work to do the work queue wake up. The worse
		 * that can happen if we race with !trace_empty() is that
		 * an event will cause an irq_work to try to wake up
		 * an empty queue.
		 *
		 * There's no reason to protect this flag either, as
		 * the work queue and irq_work logic will do the necessary
		 * synchronization for the wake ups. The only thing
		 * that is necessary is that the wake up happens after
		 * a task has been queued. It's OK for spurious wake ups.
		 */
		if (full)
			work->full_waiters_pending = true;
		else
			work->waiters_pending = true;

		if (signal_pending(current)) {
			ret = -EINTR;
			break;
		}

		if (cpu == RING_BUFFER_ALL_CPUS && !ring_buffer_empty(buffer))
			break;

		if (cpu != RING_BUFFER_ALL_CPUS &&
		    !ring_buffer_empty_cpu(buffer, cpu)) {
			unsigned long flags;
			bool pagebusy;

			if (!full)
				break;

			raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);
			pagebusy = cpu_buffer->reader_page == cpu_buffer->commit_page;
			raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);

			if (!pagebusy)
				break;
		}

		schedule();
	}

	if (full)
		finish_wait(&work->full_waiters, &wait);
	else
		finish_wait(&work->waiters, &wait);

	return ret;
}