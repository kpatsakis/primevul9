static int skcipher_wait_for_wmem(struct sock *sk, unsigned flags)
{
	long timeout;
	DEFINE_WAIT(wait);
	int err = -ERESTARTSYS;

	if (flags & MSG_DONTWAIT)
		return -EAGAIN;

	sk_set_bit(SOCKWQ_ASYNC_NOSPACE, sk);

	for (;;) {
		if (signal_pending(current))
			break;
		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		timeout = MAX_SCHEDULE_TIMEOUT;
		if (sk_wait_event(sk, &timeout, skcipher_writable(sk))) {
			err = 0;
			break;
		}
	}
	finish_wait(sk_sleep(sk), &wait);

	return err;
}