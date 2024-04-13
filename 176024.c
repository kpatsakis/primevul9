static int userfaultfd_copy(struct userfaultfd_ctx *ctx,
			    unsigned long arg)
{
	__s64 ret;
	struct uffdio_copy uffdio_copy;
	struct uffdio_copy __user *user_uffdio_copy;
	struct userfaultfd_wake_range range;

	user_uffdio_copy = (struct uffdio_copy __user *) arg;

	ret = -EFAULT;
	if (copy_from_user(&uffdio_copy, user_uffdio_copy,
			   /* don't copy "copy" last field */
			   sizeof(uffdio_copy)-sizeof(__s64)))
		goto out;

	ret = validate_range(ctx->mm, uffdio_copy.dst, uffdio_copy.len);
	if (ret)
		goto out;
	/*
	 * double check for wraparound just in case. copy_from_user()
	 * will later check uffdio_copy.src + uffdio_copy.len to fit
	 * in the userland range.
	 */
	ret = -EINVAL;
	if (uffdio_copy.src + uffdio_copy.len <= uffdio_copy.src)
		goto out;
	if (uffdio_copy.mode & ~UFFDIO_COPY_MODE_DONTWAKE)
		goto out;
	if (mmget_not_zero(ctx->mm)) {
		ret = mcopy_atomic(ctx->mm, uffdio_copy.dst, uffdio_copy.src,
				   uffdio_copy.len);
		mmput(ctx->mm);
	} else {
		return -ESRCH;
	}
	if (unlikely(put_user(ret, &user_uffdio_copy->copy)))
		return -EFAULT;
	if (ret < 0)
		goto out;
	BUG_ON(!ret);
	/* len == 0 would wake all */
	range.len = ret;
	if (!(uffdio_copy.mode & UFFDIO_COPY_MODE_DONTWAKE)) {
		range.start = uffdio_copy.dst;
		wake_userfault(ctx, &range);
	}
	ret = range.len == uffdio_copy.len ? 0 : -EAGAIN;
out:
	return ret;
}