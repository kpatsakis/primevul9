static long ioctl_doorbell(struct fsl_hv_ioctl_doorbell __user *p)
{
	struct fsl_hv_ioctl_doorbell param;

	/* Get the parameters from the user. */
	if (copy_from_user(&param, p, sizeof(struct fsl_hv_ioctl_doorbell)))
		return -EFAULT;

	param.ret = ev_doorbell_send(param.doorbell);

	if (copy_to_user(&p->ret, &param.ret, sizeof(__u32)))
		return -EFAULT;

	return 0;
}