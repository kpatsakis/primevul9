static int of_unittest_device_exists(int unittest_nr, enum overlay_type ovtype)
{
	const char *path;

	path = unittest_path(unittest_nr, ovtype);

	switch (ovtype) {
	case PDEV_OVERLAY:
		return of_path_platform_device_exists(path);
	case I2C_OVERLAY:
		return of_path_i2c_client_exists(path);
	}
	return 0;
}