static void __init of_unittest_overlay_i2c_13(void)
{
	/* device should disable */
	if (of_unittest_apply_overlay_check(13, 13, 1, 0, I2C_OVERLAY))
		return;

	unittest(1, "overlay test %d passed\n", 13);
}