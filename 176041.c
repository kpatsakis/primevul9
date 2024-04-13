static int userfaultfd_zeropage(struct userfaultfd_ctx *ctx,
				unsigned long arg)
{
	__s64 ret;
	struct uffdio_zeropage uffdio_zeropage;
	struct uffdio_zeropage __user *user_uffdio_zeropage;
	struct userfaultfd_wake_range range;

	user_uffdio_zeropage = (struct uffdio_zeropage __user *) arg;

	ret = -EFAULT;
	if (copy_from_user(&uffdio_zeropage, user_uffdio_zeropage,
			   /* don't copy "zeropage" last field */
			   sizeof(uffdio_zeropage)-sizeof(__s64)))
		goto out;

	ret = validate_range(ctx->mm, uffdio_zeropage.range.start,
			     uffdio_zeropage.range.len);
	if (ret)
		goto out;
	ret = -EINVAL;
	if (uffdio_zeropage.mode & ~UFFDIO_ZEROPAGE_MODE_DONTWAKE)
		goto out;

	if (mmget_not_zero(ctx->mm)) {
		ret = mfill_zeropage(ctx->mm, uffdio_zeropage.range.start,
				     uffdio_zeropage.range.len);
		mmput(ctx->mm);
	} else {
		return -ESRCH;
	}
	if (unlikely(put_user(ret, &user_uffdio_zeropage->zeropage)))
		return -EFAULT;
	if (ret < 0)
		goto out;
	/* len == 0 would wake all */
	BUG_ON(!ret);
	range.len = ret;
	if (!(uffdio_zeropage.mode & UFFDIO_ZEROPAGE_MODE_DONTWAKE)) {
		range.start = uffdio_zeropage.range.start;
		wake_userfault(ctx, &range);
	}
	ret = range.len == uffdio_zeropage.range.len ? 0 : -EAGAIN;
out:
	return ret;
}