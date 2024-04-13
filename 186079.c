static void __init of_unittest_overlay_11(void)
{
	int ret;

	/* device should disable */
	ret = of_unittest_apply_revert_overlay_check(11, 11, 0, 1,
			PDEV_OVERLAY);
	unittest(ret == 0, "overlay test %d failed; overlay apply\n", 11);
}