int af_alg_wait_for_data(struct sock *sk, unsigned flags)
{
	DEFINE_WAIT_FUNC(wait, woken_wake_function);
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	long timeout;
	int err = -ERESTARTSYS;

	if (flags & MSG_DONTWAIT)
		return -EAGAIN;

	sk_set_bit(SOCKWQ_ASYNC_WAITDATA, sk);

	add_wait_queue(sk_sleep(sk), &wait);
	for (;;) {
		if (signal_pending(current))
			break;
		timeout = MAX_SCHEDULE_TIMEOUT;
		if (sk_wait_event(sk, &timeout, (ctx->used || !ctx->more),
				  &wait)) {
			err = 0;
			break;
		}
	}
	remove_wait_queue(sk_sleep(sk), &wait);

	sk_clear_bit(SOCKWQ_ASYNC_WAITDATA, sk);

	return err;
}