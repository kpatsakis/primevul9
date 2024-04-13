static void skcipher_wait(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	int ctr = 0;

	while (atomic_read(&ctx->inflight) && ctr++ < 100)
		msleep(100);
}