static void invalidate_batched_entropy(void)
{
	int cpu;
	unsigned long flags;

	for_each_possible_cpu (cpu) {
		struct batched_entropy *batched_entropy;

		batched_entropy = per_cpu_ptr(&batched_entropy_u32, cpu);
		spin_lock_irqsave(&batched_entropy->batch_lock, flags);
		batched_entropy->position = 0;
		spin_unlock(&batched_entropy->batch_lock);

		batched_entropy = per_cpu_ptr(&batched_entropy_u64, cpu);
		spin_lock(&batched_entropy->batch_lock);
		batched_entropy->position = 0;
		spin_unlock_irqrestore(&batched_entropy->batch_lock, flags);
	}
}