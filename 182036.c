static u64 kvm_steal_clock(int cpu)
{
	u64 steal;
	struct kvm_steal_time *src;
	int version;

	src = &per_cpu(steal_time, cpu);
	do {
		version = src->version;
		rmb();
		steal = src->steal;
		rmb();
	} while ((version & 1) || (version != src->version));

	return steal;
}