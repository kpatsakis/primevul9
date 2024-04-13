void mremap_userfaultfd_prep(struct vm_area_struct *vma,
			     struct vm_userfaultfd_ctx *vm_ctx)
{
	struct userfaultfd_ctx *ctx;

	ctx = vma->vm_userfaultfd_ctx.ctx;
	if (ctx && (ctx->features & UFFD_FEATURE_EVENT_REMAP)) {
		vm_ctx->ctx = ctx;
		userfaultfd_ctx_get(ctx);
	}
}