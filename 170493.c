static int skcipher_accept_parent(void *private, struct sock *sk)
{
	struct skcipher_tfm *tfm = private;

	if (!tfm->has_key && crypto_skcipher_has_setkey(tfm->skcipher))
		return -ENOKEY;

	return skcipher_accept_parent_nokey(private, sk);
}