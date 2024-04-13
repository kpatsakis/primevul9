static int pcan_usb_pro_stop(struct peak_usb_device *dev)
{
	struct pcan_usb_pro_device *pdev =
			container_of(dev, struct pcan_usb_pro_device, dev);

	/* turn off ts msgs for that interface if no other dev opened */
	if (pdev->usb_if->dev_opened_count == 1)
		pcan_usb_pro_set_ts(dev, 0);

	pdev->usb_if->dev_opened_count--;

	return 0;
}