void usb_release_bos_descriptor(struct usb_device *dev)
{
	if (dev->bos) {
		kfree(dev->bos->desc);
		kfree(dev->bos);
		dev->bos = NULL;
	}
}