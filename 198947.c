static void acm_port_shutdown(struct tty_port *port)
{
	struct acm *acm = container_of(port, struct acm, port);
	struct urb *urb;
	struct acm_wb *wb;

	/*
	 * Need to grab write_lock to prevent race with resume, but no need to
	 * hold it due to the tty-port initialised flag.
	 */
	spin_lock_irq(&acm->write_lock);
	spin_unlock_irq(&acm->write_lock);

	usb_autopm_get_interface_no_resume(acm->control);
	acm->control->needs_remote_wakeup = 0;
	usb_autopm_put_interface(acm->control);

	for (;;) {
		urb = usb_get_from_anchor(&acm->delayed);
		if (!urb)
			break;
		wb = urb->context;
		wb->use = 0;
		usb_autopm_put_interface_async(acm->control);
	}

	acm_kill_urbs(acm);
}