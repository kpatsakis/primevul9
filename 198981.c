static int wait_serial_change(struct acm *acm, unsigned long arg)
{
	int rv = 0;
	DECLARE_WAITQUEUE(wait, current);
	struct async_icount old, new;

	do {
		spin_lock_irq(&acm->read_lock);
		old = acm->oldcount;
		new = acm->iocount;
		acm->oldcount = new;
		spin_unlock_irq(&acm->read_lock);

		if ((arg & TIOCM_DSR) &&
			old.dsr != new.dsr)
			break;
		if ((arg & TIOCM_CD)  &&
			old.dcd != new.dcd)
			break;
		if ((arg & TIOCM_RI) &&
			old.rng != new.rng)
			break;

		add_wait_queue(&acm->wioctl, &wait);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		remove_wait_queue(&acm->wioctl, &wait);
		if (acm->disconnected) {
			if (arg & TIOCM_CD)
				break;
			else
				rv = -ENODEV;
		} else {
			if (signal_pending(current))
				rv = -ERESTARTSYS;
		}
	} while (!rv);

	

	return rv;
}