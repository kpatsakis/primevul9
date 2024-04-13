int af_alg_accept(struct sock *sk, struct socket *newsock, bool kern)
{
	struct alg_sock *ask = alg_sk(sk);
	const struct af_alg_type *type;
	struct sock *sk2;
	unsigned int nokey;
	int err;

	lock_sock(sk);
	type = ask->type;

	err = -EINVAL;
	if (!type)
		goto unlock;

	sk2 = sk_alloc(sock_net(sk), PF_ALG, GFP_KERNEL, &alg_proto, kern);
	err = -ENOMEM;
	if (!sk2)
		goto unlock;

	sock_init_data(newsock, sk2);
	security_sock_graft(sk2, newsock);
	security_sk_clone(sk, sk2);

	err = type->accept(ask->private, sk2);

	nokey = err == -ENOKEY;
	if (nokey && type->accept_nokey)
		err = type->accept_nokey(ask->private, sk2);

	if (err)
		goto unlock;

	sk2->sk_family = PF_ALG;

	if (nokey || !ask->refcnt++)
		sock_hold(sk);
	ask->nokey_refcnt += nokey;
	alg_sk(sk2)->parent = sk;
	alg_sk(sk2)->type = type;
	alg_sk(sk2)->nokey_refcnt = nokey;

	newsock->ops = type->ops;
	newsock->state = SS_CONNECTED;

	if (nokey)
		newsock->ops = type->ops_nokey;

	err = 0;

unlock:
	release_sock(sk);

	return err;
}