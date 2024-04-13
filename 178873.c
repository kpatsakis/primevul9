int dccp_feat_insert_opts(struct dccp_sock *dp, struct dccp_request_sock *dreq,
			  struct sk_buff *skb)
{
	struct list_head *fn = dreq ? &dreq->dreq_featneg : &dp->dccps_featneg;
	struct dccp_feat_entry *pos, *next;
	u8 opt, type, len, *ptr, nn_in_nbo[DCCP_OPTVAL_MAXLEN];
	bool rpt;

	/* put entries into @skb in the order they appear in the list */
	list_for_each_entry_safe_reverse(pos, next, fn, node) {
		opt  = dccp_feat_genopt(pos);
		type = dccp_feat_type(pos->feat_num);
		rpt  = false;

		if (pos->empty_confirm) {
			len = 0;
			ptr = NULL;
		} else {
			if (type == FEAT_SP) {
				len = pos->val.sp.len;
				ptr = pos->val.sp.vec;
				rpt = pos->needs_confirm;
			} else if (type == FEAT_NN) {
				len = dccp_feat_valid_nn_length(pos->feat_num);
				ptr = nn_in_nbo;
				dccp_encode_value_var(pos->val.nn, ptr, len);
			} else {
				DCCP_BUG("unknown feature %u", pos->feat_num);
				return -1;
			}
		}
		dccp_feat_print_opt(opt, pos->feat_num, ptr, len, 0);

		if (dccp_insert_fn_opt(skb, opt, pos->feat_num, ptr, len, rpt))
			return -1;
		if (pos->needs_mandatory && dccp_insert_option_mandatory(skb))
			return -1;

		if (skb->sk->sk_state == DCCP_OPEN &&
		    (opt == DCCPO_CONFIRM_R || opt == DCCPO_CONFIRM_L)) {
			/*
			 * Confirms don't get retransmitted (6.6.3) once the
			 * connection is in state OPEN
			 */
			dccp_feat_list_pop(pos);
		} else {
			/*
			 * Enter CHANGING after transmitting the Change
			 * option (6.6.2).
			 */
			if (pos->state == FEAT_INITIALISING)
				pos->state = FEAT_CHANGING;
		}
	}
	return 0;
}