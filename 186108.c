static void __init of_unittest_overlay_4(void)
{
	/* device should disable */
	if (of_unittest_apply_overlay_check(4, 4, 0, 1, PDEV_OVERLAY))
		return;

	unittest(1, "overlay test %d passed\n", 4);
}