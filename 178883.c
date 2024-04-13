int dccp_feat_finalise_settings(struct dccp_sock *dp)
{
	struct list_head *fn = &dp->dccps_featneg;
	struct dccp_feat_entry *entry;
	int i = 2, ccids[2] = { -1, -1 };

	/*
	 * Propagating CCIDs:
	 * 1) not useful to propagate CCID settings if this host advertises more
	 *    than one CCID: the choice of CCID  may still change - if this is
	 *    the client, or if this is the server and the client sends
	 *    singleton CCID values.
	 * 2) since is that propagate_ccid changes the list, we defer changing
	 *    the sorted list until after the traversal.
	 */
	list_for_each_entry(entry, fn, node)
		if (entry->feat_num == DCCPF_CCID && entry->val.sp.len == 1)
			ccids[entry->is_local] = entry->val.sp.vec[0];
	while (i--)
		if (ccids[i] > 0 && dccp_feat_propagate_ccid(fn, ccids[i], i))
			return -1;
	dccp_feat_print_fnlist(fn);
	return 0;
}