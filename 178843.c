static void dccp_feat_printval(u8 feat_num, dccp_feat_val const *val)
{
	u8 i, type = dccp_feat_type(feat_num);

	if (val == NULL || (type == FEAT_SP && val->sp.vec == NULL))
		dccp_pr_debug_cat("(NULL)");
	else if (type == FEAT_SP)
		for (i = 0; i < val->sp.len; i++)
			dccp_pr_debug_cat("%s%u", i ? " " : "", val->sp.vec[i]);
	else if (type == FEAT_NN)
		dccp_pr_debug_cat("%llu", (unsigned long long)val->nn);
	else
		dccp_pr_debug_cat("unknown type %u", type);
}