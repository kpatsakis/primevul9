static int acm_resume(struct usb_interface *intf)
{
	struct acm *acm = usb_get_intfdata(intf);
	struct urb *urb;
	int rv = 0;

	spin_lock_irq(&acm->write_lock);

	if (--acm->susp_count)
		goto out;

	if (tty_port_initialized(&acm->port)) {
		rv = usb_submit_urb(acm->ctrlurb, GFP_ATOMIC);

		for (;;) {
			urb = usb_get_from_anchor(&acm->delayed);
			if (!urb)
				break;

			acm_start_wb(acm, urb->context);
		}

		/*
		 * delayed error checking because we must
		 * do the write path at all cost
		 */
		if (rv < 0)
			goto out;

		rv = acm_submit_read_urbs(acm, GFP_ATOMIC);
	}
out:
	spin_unlock_irq(&acm->write_lock);

	return rv;
}