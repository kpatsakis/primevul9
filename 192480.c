static int fsl_lpspi_resume(struct device *dev)
{
	int ret;

	ret = pm_runtime_force_resume(dev);
	if (ret) {
		dev_err(dev, "Error in resume: %d\n", ret);
		return ret;
	}

	pinctrl_pm_select_default_state(dev);

	return 0;
}