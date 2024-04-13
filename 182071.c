static void kvm_unlock_kick(struct arch_spinlock *lock, __ticket_t ticket)
{
	int cpu;

	add_stats(RELEASED_SLOW, 1);
	for_each_cpu(cpu, &waiting_cpus) {
		const struct kvm_lock_waiting *w = &per_cpu(klock_waiting, cpu);
		if (ACCESS_ONCE(w->lock) == lock &&
		    ACCESS_ONCE(w->want) == ticket) {
			add_stats(RELEASED_SLOW_KICKED, 1);
			kvm_kick_cpu(cpu);
			break;
		}
	}
}