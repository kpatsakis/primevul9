static void acm_process_notification(struct acm *acm, unsigned char *buf)
{
	int newctrl;
	int difference;
	unsigned long flags;
	struct usb_cdc_notification *dr = (struct usb_cdc_notification *)buf;
	unsigned char *data = buf + sizeof(struct usb_cdc_notification);

	switch (dr->bNotificationType) {
	case USB_CDC_NOTIFY_NETWORK_CONNECTION:
		dev_dbg(&acm->control->dev,
			"%s - network connection: %d\n", __func__, dr->wValue);
		break;

	case USB_CDC_NOTIFY_SERIAL_STATE:
		if (le16_to_cpu(dr->wLength) != 2) {
			dev_dbg(&acm->control->dev,
				"%s - malformed serial state\n", __func__);
			break;
		}

		newctrl = get_unaligned_le16(data);
		dev_dbg(&acm->control->dev,
			"%s - serial state: 0x%x\n", __func__, newctrl);

		if (!acm->clocal && (acm->ctrlin & ~newctrl & ACM_CTRL_DCD)) {
			dev_dbg(&acm->control->dev,
				"%s - calling hangup\n", __func__);
			tty_port_tty_hangup(&acm->port, false);
		}

		difference = acm->ctrlin ^ newctrl;
		spin_lock_irqsave(&acm->read_lock, flags);
		acm->ctrlin = newctrl;
		acm->oldcount = acm->iocount;

		if (difference & ACM_CTRL_DSR)
			acm->iocount.dsr++;
		if (difference & ACM_CTRL_DCD)
			acm->iocount.dcd++;
		if (newctrl & ACM_CTRL_BRK)
			acm->iocount.brk++;
		if (newctrl & ACM_CTRL_RI)
			acm->iocount.rng++;
		if (newctrl & ACM_CTRL_FRAMING)
			acm->iocount.frame++;
		if (newctrl & ACM_CTRL_PARITY)
			acm->iocount.parity++;
		if (newctrl & ACM_CTRL_OVERRUN)
			acm->iocount.overrun++;
		spin_unlock_irqrestore(&acm->read_lock, flags);

		if (difference)
			wake_up_all(&acm->wioctl);

		break;

	default:
		dev_dbg(&acm->control->dev,
			"%s - unknown notification %d received: index %d len %d\n",
			__func__,
			dr->bNotificationType, dr->wIndex, dr->wLength);
	}
}