static void __exit peak_usb_exit(void)
{
	int err;

	/* last chance do send any synchronous commands here */
	err = driver_for_each_device(&peak_usb_driver.drvwrap.driver, NULL,
				     NULL, peak_usb_do_device_exit);
	if (err)
		pr_err("%s: failed to stop all can devices (err %d)\n",
			PCAN_USB_DRIVER_NAME, err);

	/* deregister this driver with the USB subsystem */
	usb_deregister(&peak_usb_driver);

	pr_info("%s: PCAN-USB interfaces driver unloaded\n",
		PCAN_USB_DRIVER_NAME);
}