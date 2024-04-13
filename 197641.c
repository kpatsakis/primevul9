static int pcan_usb_fd_start(struct peak_usb_device *dev)
{
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);
	int err;

	/* set filter mode: all acceptance */
	err = pcan_usb_fd_set_filter_std(dev, -1, 0xffffffff);
	if (err)
		return err;

	/* opening first device: */
	if (pdev->usb_if->dev_opened_count == 0) {
		/* reset time_ref */
		peak_usb_init_time_ref(&pdev->usb_if->time_ref,
				       &pcan_usb_pro_fd);

		/* enable USB calibration messages */
		err = pcan_usb_fd_set_options(dev, 1,
					      PUCAN_OPTION_ERROR,
					      PCAN_UFD_FLTEXT_CALIBRATION);
	}

	pdev->usb_if->dev_opened_count++;

	/* reset cached error counters */
	pdev->bec.txerr = 0;
	pdev->bec.rxerr = 0;

	return err;
}