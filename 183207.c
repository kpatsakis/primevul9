static void pn533_recv_ack(struct urb *urb)
{
	struct pn533_usb_phy *phy = urb->context;
	struct pn533 *priv = phy->priv;
	struct pn533_cmd *cmd = priv->cmd;
	struct pn533_std_frame *in_frame;
	int rc;

	cmd->status = urb->status;

	switch (urb->status) {
	case 0:
		break; /* success */
	case -ECONNRESET:
	case -ENOENT:
		dev_dbg(&phy->udev->dev,
			"The urb has been stopped (status %d)\n",
			urb->status);
		goto sched_wq;
	case -ESHUTDOWN:
	default:
		nfc_err(&phy->udev->dev,
			"Urb failure (status %d)\n", urb->status);
		goto sched_wq;
	}

	in_frame = phy->in_urb->transfer_buffer;

	if (!pn533_rx_frame_is_ack(in_frame)) {
		nfc_err(&phy->udev->dev, "Received an invalid ack\n");
		cmd->status = -EIO;
		goto sched_wq;
	}

	rc = pn533_submit_urb_for_response(phy, GFP_ATOMIC);
	if (rc) {
		nfc_err(&phy->udev->dev,
			"usb_submit_urb failed with result %d\n", rc);
		cmd->status = rc;
		goto sched_wq;
	}

	return;

sched_wq:
	queue_work(priv->wq, &priv->cmd_complete_work);
}