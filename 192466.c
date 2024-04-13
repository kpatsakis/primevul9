static int fsl_lpspi_suspend(struct device *dev)
{
	int ret;

	pinctrl_pm_select_sleep_state(dev);
	ret = pm_runtime_force_suspend(dev);
	return ret;
}