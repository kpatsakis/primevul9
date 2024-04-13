static void disconnect_rio(struct usb_interface *intf)
{
	struct rio_usb_data *rio = usb_get_intfdata (intf);

	usb_set_intfdata (intf, NULL);
	mutex_lock(&rio500_mutex);
	if (rio) {
		usb_deregister_dev(intf, &usb_rio_class);

		mutex_lock(&(rio->lock));
		if (rio->isopen) {
			rio->isopen = 0;
			/* better let it finish - the release will do whats needed */
			rio->rio_dev = NULL;
			mutex_unlock(&(rio->lock));
			mutex_unlock(&rio500_mutex);
			return;
		}
		kfree(rio->ibuf);
		kfree(rio->obuf);

		dev_info(&intf->dev, "USB Rio disconnected.\n");

		rio->present = 0;
		mutex_unlock(&(rio->lock));
	}
	mutex_unlock(&rio500_mutex);
}