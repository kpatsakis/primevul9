static bool get_reqs_available(struct kioctx *ctx)
{
	struct kioctx_cpu *kcpu;
	bool ret = false;
	unsigned long flags;

	preempt_disable();
	kcpu = this_cpu_ptr(ctx->cpu);

	local_irq_save(flags);
	if (!kcpu->reqs_available) {
		int old, avail = atomic_read(&ctx->reqs_available);

		do {
			if (avail < ctx->req_batch)
				goto out;

			old = avail;
			avail = atomic_cmpxchg(&ctx->reqs_available,
					       avail, avail - ctx->req_batch);
		} while (avail != old);

		kcpu->reqs_available += ctx->req_batch;
	}

	ret = true;
	kcpu->reqs_available--;
out:
	local_irq_restore(flags);
	preempt_enable();
	return ret;
}