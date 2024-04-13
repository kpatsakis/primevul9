static int pcan_usb_fd_stop(struct peak_usb_device *dev)
{
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);

	/* turn off special msgs for that interface if no other dev opened */
	if (pdev->usb_if->dev_opened_count == 1)
		pcan_usb_fd_set_options(dev, 0,
					PUCAN_OPTION_ERROR,
					PCAN_UFD_FLTEXT_CALIBRATION);
	pdev->usb_if->dev_opened_count--;

	return 0;
}