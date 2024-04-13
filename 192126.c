static int __init mcheck_disable(char *str)
{
	mca_cfg.disabled = true;
	return 1;
}