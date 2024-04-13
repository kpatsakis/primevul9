static int alg_setkey(struct sock *sk, char __user *ukey,
		      unsigned int keylen)
{
	struct alg_sock *ask = alg_sk(sk);
	const struct af_alg_type *type = ask->type;
	u8 *key;
	int err;

	key = sock_kmalloc(sk, keylen, GFP_KERNEL);
	if (!key)
		return -ENOMEM;

	err = -EFAULT;
	if (copy_from_user(key, ukey, keylen))
		goto out;

	err = type->setkey(ask->private, key, keylen);

out:
	sock_kzfree_s(sk, key, keylen);

	return err;
}