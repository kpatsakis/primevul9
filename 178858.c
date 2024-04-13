static int dccp_hdlr_ack_ratio(struct sock *sk, u64 ratio, bool rx)
{
	if (rx)
		dccp_sk(sk)->dccps_r_ack_ratio = ratio;
	else
		dccp_sk(sk)->dccps_l_ack_ratio = ratio;
	return 0;
}