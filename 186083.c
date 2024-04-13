static int of_unittest_overlay_i2c_init(void)
{
	int ret;

	ret = i2c_add_driver(&unittest_i2c_dev_driver);
	if (unittest(ret == 0,
			"could not register unittest i2c device driver\n"))
		return ret;

	ret = platform_driver_register(&unittest_i2c_bus_driver);
	if (unittest(ret == 0,
			"could not register unittest i2c bus driver\n"))
		return ret;

#if IS_BUILTIN(CONFIG_I2C_MUX)
	ret = i2c_add_driver(&unittest_i2c_mux_driver);
	if (unittest(ret == 0,
			"could not register unittest i2c mux driver\n"))
		return ret;
#endif

	return 0;
}