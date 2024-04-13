static long ioctl_stop(struct fsl_hv_ioctl_stop __user *p)
{
	struct fsl_hv_ioctl_stop param;

	/* Get the parameters from the user */
	if (copy_from_user(&param, p, sizeof(struct fsl_hv_ioctl_stop)))
		return -EFAULT;

	param.ret = fh_partition_stop(param.partition);

	if (copy_to_user(&p->ret, &param.ret, sizeof(__u32)))
		return -EFAULT;

	return 0;
}