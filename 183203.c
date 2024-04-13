static void pn533_recv_response(struct urb *urb)
{
	struct pn533_usb_phy *phy = urb->context;
	struct sk_buff *skb = NULL;

	if (!urb->status) {
		skb = alloc_skb(urb->actual_length, GFP_ATOMIC);
		if (!skb) {
			nfc_err(&phy->udev->dev, "failed to alloc memory\n");
		} else {
			skb_put_data(skb, urb->transfer_buffer,
				     urb->actual_length);
		}
	}

	pn533_recv_frame(phy->priv, skb, urb->status);
}