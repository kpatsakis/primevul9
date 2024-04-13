static void pcan_usb_pro_exit(struct peak_usb_device *dev)
{
	struct pcan_usb_pro_device *pdev =
			container_of(dev, struct pcan_usb_pro_device, dev);

	/*
	 * when rmmod called before unplug and if down, should reset things
	 * before leaving
	 */
	if (dev->can.state != CAN_STATE_STOPPED) {
		/* set bus off on the corresponding channel */
		pcan_usb_pro_set_bus(dev, 0);
	}

	/* if channel #0 (only) */
	if (dev->ctrl_idx == 0) {
		/* turn off calibration message if any device were opened */
		if (pdev->usb_if->dev_opened_count > 0)
			pcan_usb_pro_set_ts(dev, 0);

		/* tell the PCAN-USB Pro device the driver is being unloaded */
		pcan_usb_pro_drv_loaded(dev, 0);
	}
}