int dup_userfaultfd(struct vm_area_struct *vma, struct list_head *fcs)
{
	struct userfaultfd_ctx *ctx = NULL, *octx;
	struct userfaultfd_fork_ctx *fctx;

	octx = vma->vm_userfaultfd_ctx.ctx;
	if (!octx || !(octx->features & UFFD_FEATURE_EVENT_FORK)) {
		vma->vm_userfaultfd_ctx = NULL_VM_UFFD_CTX;
		vma->vm_flags &= ~(VM_UFFD_WP | VM_UFFD_MISSING);
		return 0;
	}

	list_for_each_entry(fctx, fcs, list)
		if (fctx->orig == octx) {
			ctx = fctx->new;
			break;
		}

	if (!ctx) {
		fctx = kmalloc(sizeof(*fctx), GFP_KERNEL);
		if (!fctx)
			return -ENOMEM;

		ctx = kmem_cache_alloc(userfaultfd_ctx_cachep, GFP_KERNEL);
		if (!ctx) {
			kfree(fctx);
			return -ENOMEM;
		}

		atomic_set(&ctx->refcount, 1);
		ctx->flags = octx->flags;
		ctx->state = UFFD_STATE_RUNNING;
		ctx->features = octx->features;
		ctx->released = false;
		ctx->mm = vma->vm_mm;
		mmgrab(ctx->mm);

		userfaultfd_ctx_get(octx);
		fctx->orig = octx;
		fctx->new = ctx;
		list_add_tail(&fctx->list, fcs);
	}

	vma->vm_userfaultfd_ctx.ctx = ctx;
	return 0;
}