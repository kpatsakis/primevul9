static int dccp_feat_push_confirm(struct list_head *fn_list, u8 feat, u8 local,
				  dccp_feat_val *fval)
{
	struct dccp_feat_entry *new = dccp_feat_entry_new(fn_list, feat, local);

	if (new == NULL)
		return DCCP_RESET_CODE_TOO_BUSY;

	new->feat_num	     = feat;
	new->is_local	     = local;
	new->state	     = FEAT_STABLE;	/* transition in 6.6.2 */
	new->needs_confirm   = true;
	new->empty_confirm   = (fval == NULL);
	new->val.nn	     = 0;		/* zeroes the whole structure */
	if (!new->empty_confirm)
		new->val     = *fval;
	new->needs_mandatory = false;

	return 0;
}