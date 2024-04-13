static void configfs_composite_suspend(struct usb_gadget *gadget)
{
	struct usb_composite_dev *cdev;
	struct gadget_info *gi;
	unsigned long flags;

	cdev = get_gadget_data(gadget);
	if (!cdev)
		return;

	gi = container_of(cdev, struct gadget_info, cdev);
	spin_lock_irqsave(&gi->spinlock, flags);
	cdev = get_gadget_data(gadget);
	if (!cdev || gi->unbind) {
		spin_unlock_irqrestore(&gi->spinlock, flags);
		return;
	}

	composite_suspend(gadget);
	spin_unlock_irqrestore(&gi->spinlock, flags);
}