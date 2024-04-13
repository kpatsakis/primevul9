static void __init of_unittest_overlay_10(void)
{
	int ret;
	char *child_path;

	/* device should disable */
	ret = of_unittest_apply_overlay_check(10, 10, 0, 1, PDEV_OVERLAY);
	if (unittest(ret == 0,
			"overlay test %d failed; overlay application\n", 10))
		return;

	child_path = kasprintf(GFP_KERNEL, "%s/test-unittest101",
			unittest_path(10, PDEV_OVERLAY));
	if (unittest(child_path, "overlay test %d failed; kasprintf\n", 10))
		return;

	ret = of_path_device_type_exists(child_path, PDEV_OVERLAY);
	kfree(child_path);

	unittest(ret, "overlay test %d failed; no child device\n", 10);
}