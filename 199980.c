static int dccp_close_state(struct sock *sk)
{
	const int next = dccp_new_state[sk->sk_state];
	const int ns = next & DCCP_STATE_MASK;

	if (ns != sk->sk_state)
		dccp_set_state(sk, ns);

	return next & DCCP_ACTION_FIN;
}