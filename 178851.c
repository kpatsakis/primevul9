int dccp_feat_server_ccid_dependencies(struct dccp_request_sock *dreq)
{
	struct list_head *fn = &dreq->dreq_featneg;
	struct dccp_feat_entry *entry;
	u8 is_local, ccid;

	for (is_local = 0; is_local <= 1; is_local++) {
		entry = dccp_feat_list_lookup(fn, DCCPF_CCID, is_local);

		if (entry != NULL && !entry->empty_confirm)
			ccid = entry->val.sp.vec[0];
		else
			ccid = dccp_feat_default_value(DCCPF_CCID);

		if (dccp_feat_propagate_ccid(fn, ccid, is_local))
			return -1;
	}
	return 0;
}