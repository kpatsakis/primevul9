static void acm_softint(struct work_struct *work)
{
	int i;
	struct acm *acm = container_of(work, struct acm, work);

	if (test_bit(EVENT_RX_STALL, &acm->flags)) {
		if (!(usb_autopm_get_interface(acm->data))) {
			for (i = 0; i < acm->rx_buflimit; i++)
				usb_kill_urb(acm->read_urbs[i]);
			usb_clear_halt(acm->dev, acm->in);
			acm_submit_read_urbs(acm, GFP_KERNEL);
			usb_autopm_put_interface(acm->data);
		}
		clear_bit(EVENT_RX_STALL, &acm->flags);
	}

	if (test_and_clear_bit(EVENT_TTY_WAKEUP, &acm->flags))
		tty_port_tty_wakeup(&acm->port);
}