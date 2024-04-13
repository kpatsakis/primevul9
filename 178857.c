static int dccp_hdlr_ndp(struct sock *sk, u64 enable, bool rx)
{
	if (!rx)
		dccp_sk(sk)->dccps_send_ndp_count = (enable > 0);
	return 0;
}