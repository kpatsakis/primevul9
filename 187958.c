static long ioctl_status(struct fsl_hv_ioctl_status __user *p)
{
	struct fsl_hv_ioctl_status param;
	u32 status;

	/* Get the parameters from the user */
	if (copy_from_user(&param, p, sizeof(struct fsl_hv_ioctl_status)))
		return -EFAULT;

	param.ret = fh_partition_get_status(param.partition, &status);
	if (!param.ret)
		param.status = status;

	if (copy_to_user(p, &param, sizeof(struct fsl_hv_ioctl_status)))
		return -EFAULT;

	return 0;
}