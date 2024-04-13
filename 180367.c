void dccp_destroy_sock(struct sock *sk)
{
	struct dccp_sock *dp = dccp_sk(sk);

	__skb_queue_purge(&sk->sk_write_queue);
	if (sk->sk_send_head != NULL) {
		kfree_skb(sk->sk_send_head);
		sk->sk_send_head = NULL;
	}

	/* Clean up a referenced DCCP bind bucket. */
	if (inet_csk(sk)->icsk_bind_hash != NULL)
		inet_put_port(sk);

	kfree(dp->dccps_service_list);
	dp->dccps_service_list = NULL;

	if (dp->dccps_hc_rx_ackvec != NULL) {
		dccp_ackvec_free(dp->dccps_hc_rx_ackvec);
		dp->dccps_hc_rx_ackvec = NULL;
	}
	ccid_hc_rx_delete(dp->dccps_hc_rx_ccid, sk);
	dp->dccps_hc_rx_ccid = NULL;

	/* clean up feature negotiation state */
	dccp_feat_list_purge(&dp->dccps_featneg);
}