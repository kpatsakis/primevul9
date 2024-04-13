static struct file *userfaultfd_file_create(int flags)
{
	struct file *file;
	struct userfaultfd_ctx *ctx;

	BUG_ON(!current->mm);

	/* Check the UFFD_* constants for consistency.  */
	BUILD_BUG_ON(UFFD_CLOEXEC != O_CLOEXEC);
	BUILD_BUG_ON(UFFD_NONBLOCK != O_NONBLOCK);

	file = ERR_PTR(-EINVAL);
	if (flags & ~UFFD_SHARED_FCNTL_FLAGS)
		goto out;

	file = ERR_PTR(-ENOMEM);
	ctx = kmem_cache_alloc(userfaultfd_ctx_cachep, GFP_KERNEL);
	if (!ctx)
		goto out;

	atomic_set(&ctx->refcount, 1);
	ctx->flags = flags;
	ctx->features = 0;
	ctx->state = UFFD_STATE_WAIT_API;
	ctx->released = false;
	ctx->mm = current->mm;
	/* prevent the mm struct to be freed */
	mmgrab(ctx->mm);

	file = anon_inode_getfile("[userfaultfd]", &userfaultfd_fops, ctx,
				  O_RDWR | (flags & UFFD_SHARED_FCNTL_FLAGS));
	if (IS_ERR(file)) {
		mmdrop(ctx->mm);
		kmem_cache_free(userfaultfd_ctx_cachep, ctx);
	}
out:
	return file;
}