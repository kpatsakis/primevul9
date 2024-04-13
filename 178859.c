	      dccp_feat_clone_entry(struct dccp_feat_entry const *original)
{
	struct dccp_feat_entry *new;
	u8 type = dccp_feat_type(original->feat_num);

	if (type == FEAT_UNKNOWN)
		return NULL;

	new = kmemdup(original, sizeof(struct dccp_feat_entry), gfp_any());
	if (new == NULL)
		return NULL;

	if (type == FEAT_SP && dccp_feat_clone_sp_val(&new->val,
						      original->val.sp.vec,
						      original->val.sp.len)) {
		kfree(new);
		return NULL;
	}
	return new;
}