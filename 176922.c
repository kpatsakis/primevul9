struct pcan_usb_pro_interface *pcan_usb_pro_dev_if(struct peak_usb_device *dev)
{
	struct pcan_usb_pro_device *pdev =
			container_of(dev, struct pcan_usb_pro_device, dev);
	return pdev->usb_if;
}