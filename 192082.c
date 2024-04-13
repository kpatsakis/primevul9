static int mce_syscore_suspend(void)
{
	vendor_disable_error_reporting();
	return 0;
}