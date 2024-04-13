static void skcipher_free_sgl(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;

	skcipher_pull_sgl(sk, ctx->used, 1);
}