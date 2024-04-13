static int dccp_push_empty_confirm(struct list_head *fn_list, u8 feat, u8 local)
{
	return dccp_feat_push_confirm(fn_list, feat, local, NULL);
}