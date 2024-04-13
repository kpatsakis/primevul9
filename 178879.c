static int dccp_feat_default_value(u8 feat_num)
{
	int idx = dccp_feat_index(feat_num);
	/*
	 * There are no default values for unknown features, so encountering a
	 * negative index here indicates a serious problem somewhere else.
	 */
	DCCP_BUG_ON(idx < 0);

	return idx < 0 ? 0 : dccp_feat_table[idx].default_value;
}