static long ioctl_start(struct fsl_hv_ioctl_start __user *p)
{
	struct fsl_hv_ioctl_start param;

	/* Get the parameters from the user */
	if (copy_from_user(&param, p, sizeof(struct fsl_hv_ioctl_start)))
		return -EFAULT;

	param.ret = fh_partition_start(param.partition, param.entry_point,
				       param.load);

	if (copy_to_user(&p->ret, &param.ret, sizeof(__u32)))
		return -EFAULT;

	return 0;
}