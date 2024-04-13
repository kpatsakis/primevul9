static void dccp_feat_printvals(u8 feat_num, u8 *list, u8 len)
{
	u8 type = dccp_feat_type(feat_num);
	dccp_feat_val fval = { .sp.vec = list, .sp.len = len };

	if (type == FEAT_NN)
		fval.nn = dccp_decode_value_var(list, len);
	dccp_feat_printval(feat_num, &fval);
}