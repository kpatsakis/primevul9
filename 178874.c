static int dccp_hdlr_ccid(struct sock *sk, u64 ccid, bool rx)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct ccid *new_ccid = ccid_new(ccid, sk, rx);

	if (new_ccid == NULL)
		return -ENOMEM;

	if (rx) {
		ccid_hc_rx_delete(dp->dccps_hc_rx_ccid, sk);
		dp->dccps_hc_rx_ccid = new_ccid;
	} else {
		ccid_hc_tx_delete(dp->dccps_hc_tx_ccid, sk);
		dp->dccps_hc_tx_ccid = new_ccid;
	}
	return 0;
}