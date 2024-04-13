static int pn533_submit_urb_for_response(struct pn533_usb_phy *phy, gfp_t flags)
{
	phy->in_urb->complete = pn533_recv_response;

	return usb_submit_urb(phy->in_urb, flags);
}