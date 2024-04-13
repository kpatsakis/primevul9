static __always_inline void wake_userfault(struct userfaultfd_ctx *ctx,
					   struct userfaultfd_wake_range *range)
{
	unsigned seq;
	bool need_wakeup;

	/*
	 * To be sure waitqueue_active() is not reordered by the CPU
	 * before the pagetable update, use an explicit SMP memory
	 * barrier here. PT lock release or up_read(mmap_sem) still
	 * have release semantics that can allow the
	 * waitqueue_active() to be reordered before the pte update.
	 */
	smp_mb();

	/*
	 * Use waitqueue_active because it's very frequent to
	 * change the address space atomically even if there are no
	 * userfaults yet. So we take the spinlock only when we're
	 * sure we've userfaults to wake.
	 */
	do {
		seq = read_seqcount_begin(&ctx->refile_seq);
		need_wakeup = waitqueue_active(&ctx->fault_pending_wqh) ||
			waitqueue_active(&ctx->fault_wqh);
		cond_resched();
	} while (read_seqcount_retry(&ctx->refile_seq, seq));
	if (need_wakeup)
		__wake_userfault(ctx, range);
}