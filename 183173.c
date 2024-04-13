static int kiocb_cancel(struct kioctx *ctx, struct kiocb *kiocb)
{
	kiocb_cancel_fn *old, *cancel;

	/*
	 * Don't want to set kiocb->ki_cancel = KIOCB_CANCELLED unless it
	 * actually has a cancel function, hence the cmpxchg()
	 */

	cancel = ACCESS_ONCE(kiocb->ki_cancel);
	do {
		if (!cancel || cancel == KIOCB_CANCELLED)
			return -EINVAL;

		old = cancel;
		cancel = cmpxchg(&kiocb->ki_cancel, old, KIOCB_CANCELLED);
	} while (cancel != old);

	return cancel(kiocb);
}