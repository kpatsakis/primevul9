static int ems_usb_write_mode(struct ems_usb *dev, u8 mode)
{
	dev->active_params.msg.can_params.cc_params.sja1000.mode = mode;

	return ems_usb_command_msg(dev, &dev->active_params);
}