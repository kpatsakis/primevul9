static int userfaultfd_api(struct userfaultfd_ctx *ctx,
			   unsigned long arg)
{
	struct uffdio_api uffdio_api;
	void __user *buf = (void __user *)arg;
	int ret;
	__u64 features;

	ret = -EINVAL;
	if (ctx->state != UFFD_STATE_WAIT_API)
		goto out;
	ret = -EFAULT;
	if (copy_from_user(&uffdio_api, buf, sizeof(uffdio_api)))
		goto out;
	features = uffdio_api.features;
	if (uffdio_api.api != UFFD_API || (features & ~UFFD_API_FEATURES)) {
		memset(&uffdio_api, 0, sizeof(uffdio_api));
		if (copy_to_user(buf, &uffdio_api, sizeof(uffdio_api)))
			goto out;
		ret = -EINVAL;
		goto out;
	}
	/* report all available features and ioctls to userland */
	uffdio_api.features = UFFD_API_FEATURES;
	uffdio_api.ioctls = UFFD_API_IOCTLS;
	ret = -EFAULT;
	if (copy_to_user(buf, &uffdio_api, sizeof(uffdio_api)))
		goto out;
	ctx->state = UFFD_STATE_RUNNING;
	/* only enable the requested features for this uffd context */
	ctx->features = uffd_ctx_features(features);
	ret = 0;
out:
	return ret;
}