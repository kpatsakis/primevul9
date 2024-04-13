static int pcan_usb_pro_start(struct peak_usb_device *dev)
{
	struct pcan_usb_pro_device *pdev =
			container_of(dev, struct pcan_usb_pro_device, dev);
	int err;

	err = pcan_usb_pro_set_silent(dev,
				dev->can.ctrlmode & CAN_CTRLMODE_LISTENONLY);
	if (err)
		return err;

	/* filter mode: 0-> All OFF; 1->bypass */
	err = pcan_usb_pro_set_filter(dev, 1);
	if (err)
		return err;

	/* opening first device: */
	if (pdev->usb_if->dev_opened_count == 0) {
		/* reset time_ref */
		peak_usb_init_time_ref(&pdev->usb_if->time_ref, &pcan_usb_pro);

		/* ask device to send ts messages */
		err = pcan_usb_pro_set_ts(dev, 1);
	}

	pdev->usb_if->dev_opened_count++;

	return err;
}