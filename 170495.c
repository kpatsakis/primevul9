static int skcipher_wait_for_data(struct sock *sk, unsigned flags)
{
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	long timeout;
	DEFINE_WAIT(wait);
	int err = -ERESTARTSYS;

	if (flags & MSG_DONTWAIT) {
		return -EAGAIN;
	}

	sk_set_bit(SOCKWQ_ASYNC_WAITDATA, sk);

	for (;;) {
		if (signal_pending(current))
			break;
		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		timeout = MAX_SCHEDULE_TIMEOUT;
		if (sk_wait_event(sk, &timeout, ctx->used)) {
			err = 0;
			break;
		}
	}
	finish_wait(sk_sleep(sk), &wait);

	sk_clear_bit(SOCKWQ_ASYNC_WAITDATA, sk);

	return err;
}