static int dccp_feat_propagate_ccid(struct list_head *fn, u8 id, bool is_local)
{
	const struct ccid_dependency *table = dccp_feat_ccid_deps(id, is_local);
	int i, rc = (table == NULL);

	for (i = 0; rc == 0 && table[i].dependent_feat != DCCPF_RESERVED; i++)
		if (dccp_feat_type(table[i].dependent_feat) == FEAT_SP)
			rc = __feat_register_sp(fn, table[i].dependent_feat,
						    table[i].is_local,
						    table[i].is_mandatory,
						    &table[i].val, 1);
		else
			rc = __feat_register_nn(fn, table[i].dependent_feat,
						    table[i].is_mandatory,
						    table[i].val);
	return rc;
}