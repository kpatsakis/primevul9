static const char *get_krb5_cc_type_from_config(struct pwb_context *ctx)
{
	return get_conf_item_string(ctx, "krb5_ccache_type",
				    WINBIND_KRB5_CCACHE_TYPE);
}