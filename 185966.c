static int __init init_sys32_ioctl(void)
{
	sort(ioctl_pointer, ARRAY_SIZE(ioctl_pointer), sizeof(*ioctl_pointer),
		init_sys32_ioctl_cmp, NULL);
	return 0;
}