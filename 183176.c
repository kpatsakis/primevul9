static void put_reqs_available(struct kioctx *ctx, unsigned nr)
{
	struct kioctx_cpu *kcpu;
	unsigned long flags;

	preempt_disable();
	kcpu = this_cpu_ptr(ctx->cpu);

	local_irq_save(flags);
	kcpu->reqs_available += nr;

	while (kcpu->reqs_available >= ctx->req_batch * 2) {
		kcpu->reqs_available -= ctx->req_batch;
		atomic_add(ctx->req_batch, &ctx->reqs_available);
	}

	local_irq_restore(flags);
	preempt_enable();
}