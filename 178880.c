static int __dccp_feat_activate(struct sock *sk, const int idx,
				const bool is_local, dccp_feat_val const *fval)
{
	bool rx;
	u64 val;

	if (idx < 0 || idx >= DCCP_FEAT_SUPPORTED_MAX)
		return -1;
	if (dccp_feat_table[idx].activation_hdlr == NULL)
		return 0;

	if (fval == NULL) {
		val = dccp_feat_table[idx].default_value;
	} else if (dccp_feat_table[idx].reconciliation == FEAT_SP) {
		if (fval->sp.vec == NULL) {
			/*
			 * This can happen when an empty Confirm is sent
			 * for an SP (i.e. known) feature. In this case
			 * we would be using the default anyway.
			 */
			DCCP_CRIT("Feature #%d undefined: using default", idx);
			val = dccp_feat_table[idx].default_value;
		} else {
			val = fval->sp.vec[0];
		}
	} else {
		val = fval->nn;
	}

	/* Location is RX if this is a local-RX or remote-TX feature */
	rx = (is_local == (dccp_feat_table[idx].rxtx == FEAT_AT_RX));

	dccp_debug("   -> activating %s %s, %sval=%llu\n", rx ? "RX" : "TX",
		   dccp_feat_fname(dccp_feat_table[idx].feat_num),
		   fval ? "" : "default ",  (unsigned long long)val);

	return dccp_feat_table[idx].activation_hdlr(sk, val, rx);
}