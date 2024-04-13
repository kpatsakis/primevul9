struct pcan_usb_fd_if *pcan_usb_fd_dev_if(struct peak_usb_device *dev)
{
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);
	return pdev->usb_if;
}