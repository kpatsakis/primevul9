static int skcipher_accept_parent_nokey(void *private, struct sock *sk)
{
	struct skcipher_ctx *ctx;
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_tfm *tfm = private;
	struct crypto_skcipher *skcipher = tfm->skcipher;
	unsigned int len = sizeof(*ctx) + crypto_skcipher_reqsize(skcipher);

	ctx = sock_kmalloc(sk, len, GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->iv = sock_kmalloc(sk, crypto_skcipher_ivsize(skcipher),
			       GFP_KERNEL);
	if (!ctx->iv) {
		sock_kfree_s(sk, ctx, len);
		return -ENOMEM;
	}

	memset(ctx->iv, 0, crypto_skcipher_ivsize(skcipher));

	INIT_LIST_HEAD(&ctx->tsgl);
	ctx->len = len;
	ctx->used = 0;
	ctx->more = 0;
	ctx->merge = 0;
	ctx->enc = 0;
	atomic_set(&ctx->inflight, 0);
	af_alg_init_completion(&ctx->completion);

	ask->private = ctx;

	skcipher_request_set_tfm(&ctx->req, skcipher);
	skcipher_request_set_callback(&ctx->req, CRYPTO_TFM_REQ_MAY_BACKLOG,
				      af_alg_complete, &ctx->completion);

	sk->sk_destruct = skcipher_sock_destruct;

	return 0;
}