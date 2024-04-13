static int dccp_hdlr_ackvec(struct sock *sk, u64 enable, bool rx)
{
	struct dccp_sock *dp = dccp_sk(sk);

	if (rx) {
		if (enable && dp->dccps_hc_rx_ackvec == NULL) {
			dp->dccps_hc_rx_ackvec = dccp_ackvec_alloc(gfp_any());
			if (dp->dccps_hc_rx_ackvec == NULL)
				return -ENOMEM;
		} else if (!enable) {
			dccp_ackvec_free(dp->dccps_hc_rx_ackvec);
			dp->dccps_hc_rx_ackvec = NULL;
		}
	}
	return 0;
}