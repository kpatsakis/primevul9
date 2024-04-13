static void pn533_send_complete(struct urb *urb)
{
	struct pn533_usb_phy *phy = urb->context;

	switch (urb->status) {
	case 0:
		break; /* success */
	case -ECONNRESET:
	case -ENOENT:
		dev_dbg(&phy->udev->dev,
			"The urb has been stopped (status %d)\n",
			urb->status);
		break;
	case -ESHUTDOWN:
	default:
		nfc_err(&phy->udev->dev,
			"Urb failure (status %d)\n",
			urb->status);
	}
}