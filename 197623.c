static int pcan_usb_fd_decode_error(struct pcan_usb_fd_if *usb_if,
				    struct pucan_msg *rx_msg)
{
	struct pucan_error_msg *er = (struct pucan_error_msg *)rx_msg;
	struct peak_usb_device *dev = usb_if->dev[pucan_ermsg_get_channel(er)];
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);

	/* keep a trace of tx and rx error counters for later use */
	pdev->bec.txerr = er->tx_err_cnt;
	pdev->bec.rxerr = er->rx_err_cnt;

	return 0;
}