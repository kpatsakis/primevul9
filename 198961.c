static void acm_disconnect(struct usb_interface *intf)
{
	struct acm *acm = usb_get_intfdata(intf);
	struct tty_struct *tty;
	int i;

	/* sibling interface is already cleaning up */
	if (!acm)
		return;

	mutex_lock(&acm->mutex);
	acm->disconnected = true;
	if (acm->country_codes) {
		device_remove_file(&acm->control->dev,
				&dev_attr_wCountryCodes);
		device_remove_file(&acm->control->dev,
				&dev_attr_iCountryCodeRelDate);
	}
	wake_up_all(&acm->wioctl);
	device_remove_file(&acm->control->dev, &dev_attr_bmCapabilities);
	usb_set_intfdata(acm->control, NULL);
	usb_set_intfdata(acm->data, NULL);
	mutex_unlock(&acm->mutex);

	tty = tty_port_tty_get(&acm->port);
	if (tty) {
		tty_vhangup(tty);
		tty_kref_put(tty);
	}

	acm_kill_urbs(acm);
	cancel_work_sync(&acm->work);

	tty_unregister_device(acm_tty_driver, acm->minor);

	usb_free_urb(acm->ctrlurb);
	for (i = 0; i < ACM_NW; i++)
		usb_free_urb(acm->wb[i].urb);
	for (i = 0; i < acm->rx_buflimit; i++)
		usb_free_urb(acm->read_urbs[i]);
	acm_write_buffers_free(acm);
	usb_free_coherent(acm->dev, acm->ctrlsize, acm->ctrl_buffer, acm->ctrl_dma);
	acm_read_buffers_free(acm);

	kfree(acm->notification_buffer);

	if (!acm->combined_interfaces)
		usb_driver_release_interface(&acm_driver, intf == acm->control ?
					acm->data : acm->control);

	tty_port_put(&acm->port);
}