static int acm_reset_resume(struct usb_interface *intf)
{
	struct acm *acm = usb_get_intfdata(intf);

	if (tty_port_initialized(&acm->port))
		tty_port_tty_hangup(&acm->port, false);

	return acm_resume(intf);
}