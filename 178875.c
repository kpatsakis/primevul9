u64 dccp_feat_nn_get(struct sock *sk, u8 feat)
{
	if (dccp_feat_type(feat) == FEAT_NN) {
		struct dccp_sock *dp = dccp_sk(sk);
		struct dccp_feat_entry *entry;

		entry = dccp_feat_list_lookup(&dp->dccps_featneg, feat, 1);
		if (entry != NULL)
			return entry->val.nn;

		switch (feat) {
		case DCCPF_ACK_RATIO:
			return dp->dccps_l_ack_ratio;
		case DCCPF_SEQUENCE_WINDOW:
			return dp->dccps_l_seq_win;
		}
	}
	DCCP_BUG("attempt to look up unsupported feature %u", feat);
	return 0;
}