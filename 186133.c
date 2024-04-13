static void __init of_unittest_overlay_5(void)
{
	/* device should disable */
	if (of_unittest_apply_revert_overlay_check(5, 5, 0, 1, PDEV_OVERLAY))
		return;

	unittest(1, "overlay test %d passed\n", 5);
}