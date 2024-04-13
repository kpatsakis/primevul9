static int rds_set_bool_option(unsigned char *optvar, char __user *optval,
			       int optlen)
{
	int value;

	if (optlen < sizeof(int))
		return -EINVAL;
	if (get_user(value, (int __user *) optval))
		return -EFAULT;
	*optvar = !!value;
	return 0;
}