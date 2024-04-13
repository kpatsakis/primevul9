static void configfs_composite_unbind(struct usb_gadget *gadget)
{
	struct usb_composite_dev	*cdev;
	struct gadget_info		*gi;
	unsigned long flags;

	/* the gi->lock is hold by the caller */

	cdev = get_gadget_data(gadget);
	gi = container_of(cdev, struct gadget_info, cdev);
	spin_lock_irqsave(&gi->spinlock, flags);
	gi->unbind = 1;
	spin_unlock_irqrestore(&gi->spinlock, flags);

	kfree(otg_desc[0]);
	otg_desc[0] = NULL;
	purge_configs_funcs(gi);
	composite_dev_cleanup(cdev);
	usb_ep_autoconfig_reset(cdev->gadget);
	spin_lock_irqsave(&gi->spinlock, flags);
	cdev->gadget = NULL;
	set_gadget_data(gadget, NULL);
	spin_unlock_irqrestore(&gi->spinlock, flags);
}