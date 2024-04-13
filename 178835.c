int dccp_feat_signal_nn_change(struct sock *sk, u8 feat, u64 nn_val)
{
	struct list_head *fn = &dccp_sk(sk)->dccps_featneg;
	dccp_feat_val fval = { .nn = nn_val };
	struct dccp_feat_entry *entry;

	if (sk->sk_state != DCCP_OPEN && sk->sk_state != DCCP_PARTOPEN)
		return 0;

	if (dccp_feat_type(feat) != FEAT_NN ||
	    !dccp_feat_is_valid_nn_val(feat, nn_val))
		return -EINVAL;

	if (nn_val == dccp_feat_nn_get(sk, feat))
		return 0;	/* already set or negotiation under way */

	entry = dccp_feat_list_lookup(fn, feat, 1);
	if (entry != NULL) {
		dccp_pr_debug("Clobbering existing NN entry %llu -> %llu\n",
			      (unsigned long long)entry->val.nn,
			      (unsigned long long)nn_val);
		dccp_feat_list_pop(entry);
	}

	inet_csk_schedule_ack(sk);
	return dccp_feat_push_change(fn, feat, 1, 0, &fval);
}