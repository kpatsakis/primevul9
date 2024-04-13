static int __feat_register_nn(struct list_head *fn, u8 feat,
			      u8 mandatory, u64 nn_val)
{
	dccp_feat_val fval = { .nn = nn_val };

	if (dccp_feat_type(feat) != FEAT_NN ||
	    !dccp_feat_is_valid_nn_val(feat, nn_val))
		return -EINVAL;

	/* Don't bother with default values, they will be activated anyway. */
	if (nn_val - (u64)dccp_feat_default_value(feat) == 0)
		return 0;

	return dccp_feat_push_change(fn, feat, 1, mandatory, &fval);
}