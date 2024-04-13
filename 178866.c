static void dccp_feat_print_entry(struct dccp_feat_entry const *entry)
{
	dccp_debug("   * %s %s = ", entry->is_local ? "local" : "remote",
				    dccp_feat_fname(entry->feat_num));
	dccp_feat_printval(entry->feat_num, &entry->val);
	dccp_pr_debug_cat(", state=%s %s\n", dccp_feat_sname[entry->state],
			  entry->needs_confirm ? "(Confirm pending)" : "");
}