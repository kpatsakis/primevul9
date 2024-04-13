static void alg_sock_destruct(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);

	alg_do_release(ask->type, ask->private);
}