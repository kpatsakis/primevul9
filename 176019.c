int userfaultfd_unmap_prep(struct vm_area_struct *vma,
			   unsigned long start, unsigned long end,
			   struct list_head *unmaps)
{
	for ( ; vma && vma->vm_start < end; vma = vma->vm_next) {
		struct userfaultfd_unmap_ctx *unmap_ctx;
		struct userfaultfd_ctx *ctx = vma->vm_userfaultfd_ctx.ctx;

		if (!ctx || !(ctx->features & UFFD_FEATURE_EVENT_UNMAP) ||
		    has_unmap_ctx(ctx, unmaps, start, end))
			continue;

		unmap_ctx = kzalloc(sizeof(*unmap_ctx), GFP_KERNEL);
		if (!unmap_ctx)
			return -ENOMEM;

		userfaultfd_ctx_get(ctx);
		unmap_ctx->ctx = ctx;
		unmap_ctx->start = start;
		unmap_ctx->end = end;
		list_add_tail(&unmap_ctx->list, unmaps);
	}

	return 0;
}