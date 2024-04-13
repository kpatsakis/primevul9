static int alg_setsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	const struct af_alg_type *type;
	int err = -EBUSY;

	lock_sock(sk);
	if (ask->refcnt)
		goto unlock;

	type = ask->type;

	err = -ENOPROTOOPT;
	if (level != SOL_ALG || !type)
		goto unlock;

	switch (optname) {
	case ALG_SET_KEY:
		if (sock->state == SS_CONNECTED)
			goto unlock;
		if (!type->setkey)
			goto unlock;

		err = alg_setkey(sk, optval, optlen);
		break;
	case ALG_SET_AEAD_AUTHSIZE:
		if (sock->state == SS_CONNECTED)
			goto unlock;
		if (!type->setauthsize)
			goto unlock;
		err = type->setauthsize(ask->private, optlen);
	}

unlock:
	release_sock(sk);

	return err;
}