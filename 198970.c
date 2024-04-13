static void acm_read_bulk_callback(struct urb *urb)
{
	struct acm_rb *rb = urb->context;
	struct acm *acm = rb->instance;
	int status = urb->status;
	bool stopped = false;
	bool stalled = false;

	dev_vdbg(&acm->data->dev, "got urb %d, len %d, status %d\n",
		rb->index, urb->actual_length, status);

	if (!acm->dev) {
		dev_dbg(&acm->data->dev, "%s - disconnected\n", __func__);
		return;
	}

	switch (status) {
	case 0:
		usb_mark_last_busy(acm->dev);
		acm_process_read_urb(acm, urb);
		break;
	case -EPIPE:
		set_bit(EVENT_RX_STALL, &acm->flags);
		stalled = true;
		break;
	case -ENOENT:
	case -ECONNRESET:
	case -ESHUTDOWN:
		dev_dbg(&acm->data->dev,
			"%s - urb shutting down with status: %d\n",
			__func__, status);
		stopped = true;
		break;
	default:
		dev_dbg(&acm->data->dev,
			"%s - nonzero urb status received: %d\n",
			__func__, status);
		break;
	}

	/*
	 * Make sure URB processing is done before marking as free to avoid
	 * racing with unthrottle() on another CPU. Matches the barriers
	 * implied by the test_and_clear_bit() in acm_submit_read_urb().
	 */
	smp_mb__before_atomic();
	set_bit(rb->index, &acm->read_urbs_free);
	/*
	 * Make sure URB is marked as free before checking the throttled flag
	 * to avoid racing with unthrottle() on another CPU. Matches the
	 * smp_mb() in unthrottle().
	 */
	smp_mb__after_atomic();

	if (stopped || stalled) {
		if (stalled)
			schedule_work(&acm->work);
		return;
	}

	if (test_bit(ACM_THROTTLED, &acm->flags))
		return;

	acm_submit_read_urb(acm, rb->index, GFP_ATOMIC);
}