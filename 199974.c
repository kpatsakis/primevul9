int dccp_destroy_sock(struct sock *sk)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct dccp_minisock *dmsk = dccp_msk(sk);

	/*
	 * DCCP doesn't use sk_write_queue, just sk_send_head
	 * for retransmissions
	 */
	if (sk->sk_send_head != NULL) {
		kfree_skb(sk->sk_send_head);
		sk->sk_send_head = NULL;
	}

	/* Clean up a referenced DCCP bind bucket. */
	if (inet_csk(sk)->icsk_bind_hash != NULL)
		inet_put_port(&dccp_hashinfo, sk);

	kfree(dp->dccps_service_list);
	dp->dccps_service_list = NULL;

	if (dmsk->dccpms_send_ack_vector) {
		dccp_ackvec_free(dp->dccps_hc_rx_ackvec);
		dp->dccps_hc_rx_ackvec = NULL;
	}
	ccid_hc_rx_delete(dp->dccps_hc_rx_ccid, sk);
	ccid_hc_tx_delete(dp->dccps_hc_tx_ccid, sk);
	dp->dccps_hc_rx_ccid = dp->dccps_hc_tx_ccid = NULL;

	/* clean up feature negotiation state */
	dccp_feat_clean(dmsk);

	return 0;
}