u32 get_random_u32(void)
{
	u32 ret;
	unsigned long flags;
	struct batched_entropy *batch;
	static void *previous;

	warn_unseeded_randomness(&previous);

	batch = raw_cpu_ptr(&batched_entropy_u32);
	spin_lock_irqsave(&batch->batch_lock, flags);
	if (batch->position % ARRAY_SIZE(batch->entropy_u32) == 0) {
		extract_crng((u8 *)batch->entropy_u32);
		batch->position = 0;
	}
	ret = batch->entropy_u32[batch->position++];
	spin_unlock_irqrestore(&batch->batch_lock, flags);
	return ret;
}