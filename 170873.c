static inline bool rb_reader_lock(struct ring_buffer_per_cpu *cpu_buffer)
{
	if (likely(!in_nmi())) {
		raw_spin_lock(&cpu_buffer->reader_lock);
		return true;
	}

	/*
	 * If an NMI die dumps out the content of the ring buffer
	 * trylock must be used to prevent a deadlock if the NMI
	 * preempted a task that holds the ring buffer locks. If
	 * we get the lock then all is fine, if not, then continue
	 * to do the read, but this can corrupt the ring buffer,
	 * so it must be permanently disabled from future writes.
	 * Reading from NMI is a oneshot deal.
	 */
	if (raw_spin_trylock(&cpu_buffer->reader_lock))
		return true;

	/* Continue without locking, but disable the ring buffer */
	atomic_inc(&cpu_buffer->record_disabled);
	return false;
}