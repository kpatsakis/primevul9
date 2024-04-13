static void __init of_unittest_overlay_2(void)
{
	/* device should enable */
	if (of_unittest_apply_overlay_check(2, 2, 0, 1, PDEV_OVERLAY))
		return;

	unittest(1, "overlay test %d passed\n", 2);
}