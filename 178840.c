static int dccp_feat_clone_sp_val(dccp_feat_val *fval, u8 const *val, u8 len)
{
	fval->sp.len = len;
	if (fval->sp.len > 0) {
		fval->sp.vec = kmemdup(val, len, gfp_any());
		if (fval->sp.vec == NULL) {
			fval->sp.len = 0;
			return -ENOBUFS;
		}
	}
	return 0;
}