static int dccp_feat_reconcile(dccp_feat_val *fv, u8 *arr, u8 len,
			       bool is_server, bool reorder)
{
	int rc;

	if (!fv->sp.vec || !arr) {
		DCCP_CRIT("NULL feature value or array");
		return 0;
	}

	if (is_server)
		rc = dccp_feat_preflist_match(fv->sp.vec, fv->sp.len, arr, len);
	else
		rc = dccp_feat_preflist_match(arr, len, fv->sp.vec, fv->sp.len);

	if (!reorder)
		return rc;
	if (rc < 0)
		return 0;

	/*
	 * Reorder list: used for activating features and in dccp_insert_fn_opt.
	 */
	return dccp_feat_prefer(rc, fv->sp.vec, fv->sp.len);
}