static int acm_port_activate(struct tty_port *port, struct tty_struct *tty)
{
	struct acm *acm = container_of(port, struct acm, port);
	int retval = -ENODEV;
	int i;

	mutex_lock(&acm->mutex);
	if (acm->disconnected)
		goto disconnected;

	retval = usb_autopm_get_interface(acm->control);
	if (retval)
		goto error_get_interface;

	/*
	 * FIXME: Why do we need this? Allocating 64K of physically contiguous
	 * memory is really nasty...
	 */
	set_bit(TTY_NO_WRITE_SPLIT, &tty->flags);
	acm->control->needs_remote_wakeup = 1;

	acm->ctrlurb->dev = acm->dev;
	retval = usb_submit_urb(acm->ctrlurb, GFP_KERNEL);
	if (retval) {
		dev_err(&acm->control->dev,
			"%s - usb_submit_urb(ctrl irq) failed\n", __func__);
		goto error_submit_urb;
	}

	acm_tty_set_termios(tty, NULL);

	/*
	 * Unthrottle device in case the TTY was closed while throttled.
	 */
	clear_bit(ACM_THROTTLED, &acm->flags);

	retval = acm_submit_read_urbs(acm, GFP_KERNEL);
	if (retval)
		goto error_submit_read_urbs;

	usb_autopm_put_interface(acm->control);

	mutex_unlock(&acm->mutex);

	return 0;

error_submit_read_urbs:
	for (i = 0; i < acm->rx_buflimit; i++)
		usb_kill_urb(acm->read_urbs[i]);
	usb_kill_urb(acm->ctrlurb);
error_submit_urb:
	usb_autopm_put_interface(acm->control);
error_get_interface:
disconnected:
	mutex_unlock(&acm->mutex);

	return usb_translate_errors(retval);
}