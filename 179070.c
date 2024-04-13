static int __init peak_usb_init(void)
{
	int err;

	/* register this driver with the USB subsystem */
	err = usb_register(&peak_usb_driver);
	if (err)
		pr_err("%s: usb_register failed (err %d)\n",
			PCAN_USB_DRIVER_NAME, err);

	return err;
}