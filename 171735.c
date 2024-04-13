static int alg_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	const u32 allowed = CRYPTO_ALG_KERN_DRIVER_ONLY;
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct sockaddr_alg *sa = (void *)uaddr;
	const struct af_alg_type *type;
	void *private;
	int err;

	if (sock->state == SS_CONNECTED)
		return -EINVAL;

	if (addr_len < sizeof(*sa))
		return -EINVAL;

	/* If caller uses non-allowed flag, return error. */
	if ((sa->salg_feat & ~allowed) || (sa->salg_mask & ~allowed))
		return -EINVAL;

	sa->salg_type[sizeof(sa->salg_type) - 1] = 0;
	sa->salg_name[sizeof(sa->salg_name) + addr_len - sizeof(*sa) - 1] = 0;

	type = alg_get_type(sa->salg_type);
	if (IS_ERR(type) && PTR_ERR(type) == -ENOENT) {
		request_module("algif-%s", sa->salg_type);
		type = alg_get_type(sa->salg_type);
	}

	if (IS_ERR(type))
		return PTR_ERR(type);

	private = type->bind(sa->salg_name, sa->salg_feat, sa->salg_mask);
	if (IS_ERR(private)) {
		module_put(type->owner);
		return PTR_ERR(private);
	}

	err = -EBUSY;
	lock_sock(sk);
	if (ask->refcnt | ask->nokey_refcnt)
		goto unlock;

	swap(ask->type, type);
	swap(ask->private, private);

	err = 0;

unlock:
	release_sock(sk);

	alg_do_release(type, private);

	return err;
}