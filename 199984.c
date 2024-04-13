static inline int dccp_listen_start(struct sock *sk, int backlog)
{
	struct dccp_sock *dp = dccp_sk(sk);

	dp->dccps_role = DCCP_ROLE_LISTEN;
	return inet_csk_listen_start(sk, backlog);
}