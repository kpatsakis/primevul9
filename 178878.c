static int dccp_feat_push_change(struct list_head *fn_list, u8 feat, u8 local,
				 u8 mandatory, dccp_feat_val *fval)
{
	struct dccp_feat_entry *new = dccp_feat_entry_new(fn_list, feat, local);

	if (new == NULL)
		return -ENOMEM;

	new->feat_num	     = feat;
	new->is_local	     = local;
	new->state	     = FEAT_INITIALISING;
	new->needs_confirm   = false;
	new->empty_confirm   = false;
	new->val	     = *fval;
	new->needs_mandatory = mandatory;

	return 0;
}