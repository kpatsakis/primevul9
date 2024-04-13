static u8 dccp_feat_type(u8 feat_num)
{
	int idx = dccp_feat_index(feat_num);

	if (idx < 0)
		return FEAT_UNKNOWN;
	return dccp_feat_table[idx].reconciliation;
}