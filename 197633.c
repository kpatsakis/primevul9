static void pcan_usb_fd_exit(struct peak_usb_device *dev)
{
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);

	/* when rmmod called before unplug and if down, should reset things
	 * before leaving
	 */
	if (dev->can.state != CAN_STATE_STOPPED) {
		/* set bus off on the corresponding channel */
		pcan_usb_fd_set_bus(dev, 0);
	}

	/* switch off corresponding CAN LEDs */
	pcan_usb_fd_set_can_led(dev, PCAN_UFD_LED_OFF);

	/* if channel #0 (only) */
	if (dev->ctrl_idx == 0) {
		/* turn off calibration message if any device were opened */
		if (pdev->usb_if->dev_opened_count > 0)
			pcan_usb_fd_set_options(dev, 0,
						PUCAN_OPTION_ERROR,
						PCAN_UFD_FLTEXT_CALIBRATION);

		/* tell USB adapter that the driver is being unloaded */
		pcan_usb_fd_drv_loaded(dev, 0);
	}
}