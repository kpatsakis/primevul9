static int configfs_composite_setup(struct usb_gadget *gadget,
		const struct usb_ctrlrequest *ctrl)
{
	struct usb_composite_dev *cdev;
	struct gadget_info *gi;
	unsigned long flags;
	int ret;

	cdev = get_gadget_data(gadget);
	if (!cdev)
		return 0;

	gi = container_of(cdev, struct gadget_info, cdev);
	spin_lock_irqsave(&gi->spinlock, flags);
	cdev = get_gadget_data(gadget);
	if (!cdev || gi->unbind) {
		spin_unlock_irqrestore(&gi->spinlock, flags);
		return 0;
	}

	ret = composite_setup(gadget, ctrl);
	spin_unlock_irqrestore(&gi->spinlock, flags);
	return ret;
}