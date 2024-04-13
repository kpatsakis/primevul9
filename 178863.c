static int dccp_hdlr_seq_win(struct sock *sk, u64 seq_win, bool rx)
{
	struct dccp_sock *dp = dccp_sk(sk);

	if (rx) {
		dp->dccps_r_seq_win = seq_win;
		/* propagate changes to update SWL/SWH */
		dccp_update_gsr(sk, dp->dccps_gsr);
	} else {
		dp->dccps_l_seq_win = seq_win;
		/* propagate changes to update AWL */
		dccp_update_gss(sk, dp->dccps_gss);
	}
	return 0;
}