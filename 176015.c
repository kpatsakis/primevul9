static int userfaultfd_wake(struct userfaultfd_ctx *ctx,
			    unsigned long arg)
{
	int ret;
	struct uffdio_range uffdio_wake;
	struct userfaultfd_wake_range range;
	const void __user *buf = (void __user *)arg;

	ret = -EFAULT;
	if (copy_from_user(&uffdio_wake, buf, sizeof(uffdio_wake)))
		goto out;

	ret = validate_range(ctx->mm, uffdio_wake.start, uffdio_wake.len);
	if (ret)
		goto out;

	range.start = uffdio_wake.start;
	range.len = uffdio_wake.len;

	/*
	 * len == 0 means wake all and we don't want to wake all here,
	 * so check it again to be sure.
	 */
	VM_BUG_ON(!range.len);

	wake_userfault(ctx, &range);
	ret = 0;

out:
	return ret;
}