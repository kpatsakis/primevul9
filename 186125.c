static int of_path_platform_device_exists(const char *path)
{
	struct platform_device *pdev;

	pdev = of_path_to_platform_device(path);
	platform_device_put(pdev);
	return pdev != NULL;
}