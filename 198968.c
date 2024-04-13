static int acm_pre_reset(struct usb_interface *intf)
{
	struct acm *acm = usb_get_intfdata(intf);

	clear_bit(EVENT_RX_STALL, &acm->flags);
	acm->nb_index = 0; /* pending control transfers are lost */

	return 0;
}