static void userfaultfd_ctx_put(struct userfaultfd_ctx *ctx)
{
	if (atomic_dec_and_test(&ctx->refcount)) {
		VM_BUG_ON(spin_is_locked(&ctx->fault_pending_wqh.lock));
		VM_BUG_ON(waitqueue_active(&ctx->fault_pending_wqh));
		VM_BUG_ON(spin_is_locked(&ctx->fault_wqh.lock));
		VM_BUG_ON(waitqueue_active(&ctx->fault_wqh));
		VM_BUG_ON(spin_is_locked(&ctx->event_wqh.lock));
		VM_BUG_ON(waitqueue_active(&ctx->event_wqh));
		VM_BUG_ON(spin_is_locked(&ctx->fd_wqh.lock));
		VM_BUG_ON(waitqueue_active(&ctx->fd_wqh));
		mmdrop(ctx->mm);
		kmem_cache_free(userfaultfd_ctx_cachep, ctx);
	}
}