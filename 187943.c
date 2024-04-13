static long ioctl_restart(struct fsl_hv_ioctl_restart __user *p)
{
	struct fsl_hv_ioctl_restart param;

	/* Get the parameters from the user */
	if (copy_from_user(&param, p, sizeof(struct fsl_hv_ioctl_restart)))
		return -EFAULT;

	param.ret = fh_partition_restart(param.partition);

	if (copy_to_user(&p->ret, &param.ret, sizeof(__u32)))
		return -EFAULT;

	return 0;
}