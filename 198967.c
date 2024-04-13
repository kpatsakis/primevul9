static void acm_write_bulk(struct urb *urb)
{
	struct acm_wb *wb = urb->context;
	struct acm *acm = wb->instance;
	unsigned long flags;
	int status = urb->status;

	if (status || (urb->actual_length != urb->transfer_buffer_length))
		dev_vdbg(&acm->data->dev, "wrote len %d/%d, status %d\n",
			urb->actual_length,
			urb->transfer_buffer_length,
			status);

	spin_lock_irqsave(&acm->write_lock, flags);
	acm_write_done(acm, wb);
	spin_unlock_irqrestore(&acm->write_lock, flags);
	set_bit(EVENT_TTY_WAKEUP, &acm->flags);
	schedule_work(&acm->work);
}