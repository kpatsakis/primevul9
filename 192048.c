static void rtl8xxxu_int_complete(struct urb *urb)
{
	struct rtl8xxxu_priv *priv = (struct rtl8xxxu_priv *)urb->context;
	struct device *dev = &priv->udev->dev;
	int ret;

	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_INTERRUPT)
		dev_dbg(dev, "%s: status %i\n", __func__, urb->status);
	if (urb->status == 0) {
		usb_anchor_urb(urb, &priv->int_anchor);
		ret = usb_submit_urb(urb, GFP_ATOMIC);
		if (ret)
			usb_unanchor_urb(urb);
	} else {
		dev_dbg(dev, "%s: Error %i\n", __func__, urb->status);
	}
}