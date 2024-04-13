static __init int vdso_setup(char *s)
{
	vdso64_enabled = simple_strtoul(s, NULL, 0);
	return 0;
}