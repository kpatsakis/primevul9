static u8 dccp_feat_sp_list_ok(u8 feat_num, u8 const *sp_list, u8 sp_len)
{
	if (sp_list == NULL || sp_len < 1)
		return 0;
	while (sp_len--)
		if (!dccp_feat_is_valid_sp_val(feat_num, *sp_list++))
			return 0;
	return 1;
}