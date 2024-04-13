static int __init init_sys32_ioctl_cmp(const void *p, const void *q)
{
	unsigned int a, b;
	a = *(unsigned int *)p;
	b = *(unsigned int *)q;
	if (a > b)
		return 1;
	if (a < b)
		return -1;
	return 0;
}