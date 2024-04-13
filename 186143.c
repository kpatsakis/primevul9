static int of_path_device_type_exists(const char *path,
		enum overlay_type ovtype)
{
	switch (ovtype) {
	case PDEV_OVERLAY:
		return of_path_platform_device_exists(path);
	case I2C_OVERLAY:
		return of_path_i2c_client_exists(path);
	}
	return 0;
}