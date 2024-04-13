static const char *get_member_from_config(struct pwb_context *ctx)
{
	const char *ret = NULL;
	ret = get_conf_item_string(ctx, "require_membership_of",
				   WINBIND_REQUIRED_MEMBERSHIP);
	if (ret) {
		return ret;
	}
	return get_conf_item_string(ctx, "require-membership-of",
				    WINBIND_REQUIRED_MEMBERSHIP);
}