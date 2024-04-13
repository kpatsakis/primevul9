static int acm_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct acm *acm = usb_get_intfdata(intf);
	int cnt;

	spin_lock_irq(&acm->write_lock);
	if (PMSG_IS_AUTO(message)) {
		if (acm->transmitting) {
			spin_unlock_irq(&acm->write_lock);
			return -EBUSY;
		}
	}
	cnt = acm->susp_count++;
	spin_unlock_irq(&acm->write_lock);

	if (cnt)
		return 0;

	acm_kill_urbs(acm);
	cancel_work_sync(&acm->work);

	return 0;
}