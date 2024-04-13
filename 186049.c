static int ioctl_fibmap(struct file *filp, int __user *p)
{
	struct address_space *mapping = filp->f_mapping;
	int res, block;

	/* do we support this mess? */
	if (!mapping->a_ops->bmap)
		return -EINVAL;
	if (!capable(CAP_SYS_RAWIO))
		return -EPERM;
	res = get_user(block, p);
	if (res)
		return res;
	res = mapping->a_ops->bmap(mapping, block);
	return put_user(res, p);
}