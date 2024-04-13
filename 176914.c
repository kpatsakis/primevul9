static void pcan_usb_pro_free(struct peak_usb_device *dev)
{
	/* last device: can free pcan_usb_pro_interface object now */
	if (!dev->prev_siblings && !dev->next_siblings)
		kfree(pcan_usb_pro_dev_if(dev));
}