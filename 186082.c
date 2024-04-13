static void __init of_unittest_overlay_3(void)
{
	/* device should disable */
	if (of_unittest_apply_overlay_check(3, 3, 1, 0, PDEV_OVERLAY))
		return;

	unittest(1, "overlay test %d passed\n", 3);
}