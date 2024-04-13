static int hidpp10_enable_battery_reporting(struct hidpp_device *hidpp_dev)
{
	return hidpp10_set_register(hidpp_dev, HIDPP_REG_ENABLE_REPORTS, 0,
			  HIDPP_ENABLE_BAT_REPORT, HIDPP_ENABLE_BAT_REPORT);
}