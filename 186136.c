static void of_unittest_overlay_i2c_cleanup(void)
{
#if IS_BUILTIN(CONFIG_I2C_MUX)
	i2c_del_driver(&unittest_i2c_mux_driver);
#endif
	platform_driver_unregister(&unittest_i2c_bus_driver);
	i2c_del_driver(&unittest_i2c_dev_driver);
}