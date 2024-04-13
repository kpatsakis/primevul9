void af_alg_release_parent(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	unsigned int nokey = ask->nokey_refcnt;
	bool last = nokey && !ask->refcnt;

	sk = ask->parent;
	ask = alg_sk(sk);

	lock_sock(sk);
	ask->nokey_refcnt -= nokey;
	if (!last)
		last = !--ask->refcnt;
	release_sock(sk);

	if (last)
		sock_put(sk);
}