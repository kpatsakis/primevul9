static void skcipher_sock_destruct(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(&ctx->req);

	if (atomic_read(&ctx->inflight))
		skcipher_wait(sk);

	skcipher_free_sgl(sk);
	sock_kzfree_s(sk, ctx->iv, crypto_skcipher_ivsize(tfm));
	sock_kfree_s(sk, ctx, ctx->len);
	af_alg_release_parent(sk);
}