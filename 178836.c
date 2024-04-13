int dccp_feat_parse_options(struct sock *sk, struct dccp_request_sock *dreq,
			    u8 mandatory, u8 opt, u8 feat, u8 *val, u8 len)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct list_head *fn = dreq ? &dreq->dreq_featneg : &dp->dccps_featneg;
	bool server = false;

	switch (sk->sk_state) {
	/*
	 *	Negotiation during connection setup
	 */
	case DCCP_LISTEN:
		server = true;			/* fall through */
	case DCCP_REQUESTING:
		switch (opt) {
		case DCCPO_CHANGE_L:
		case DCCPO_CHANGE_R:
			return dccp_feat_change_recv(fn, mandatory, opt, feat,
						     val, len, server);
		case DCCPO_CONFIRM_R:
		case DCCPO_CONFIRM_L:
			return dccp_feat_confirm_recv(fn, mandatory, opt, feat,
						      val, len, server);
		}
		break;
	/*
	 *	Support for exchanging NN options on an established connection.
	 */
	case DCCP_OPEN:
	case DCCP_PARTOPEN:
		return dccp_feat_handle_nn_established(sk, mandatory, opt, feat,
						       val, len);
	}
	return 0;	/* ignore FN options in all other states */
}