static int dccp_feat_activate(struct sock *sk, u8 feat_num, bool local,
			      dccp_feat_val const *fval)
{
	return __dccp_feat_activate(sk, dccp_feat_index(feat_num), local, fval);
}