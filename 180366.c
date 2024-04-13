static void dccp_sk_destruct(struct sock *sk)
{
	struct dccp_sock *dp = dccp_sk(sk);

	ccid_hc_tx_delete(dp->dccps_hc_tx_ccid, sk);
	dp->dccps_hc_tx_ccid = NULL;
	inet_sock_destruct(sk);
}