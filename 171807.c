static void _pam_log_state_datum(struct pwb_context *ctx,
				 int item_type,
				 const char *key,
				 int is_string)
{
	const void *data = NULL;
	if (item_type != 0) {
		pam_get_item(ctx->pamh, item_type, &data);
	} else {
		pam_get_data(ctx->pamh, key, &data);
	}
	if (data != NULL) {
		const char *type = (item_type != 0) ? "ITEM" : "DATA";
		if (is_string != 0) {
			_pam_log_debug(ctx, LOG_DEBUG,
				       "[pamh: %p] STATE: %s(%s) = \"%s\" (%p)",
				       ctx->pamh, type, key, (const char *)data,
				       data);
		} else {
			_pam_log_debug(ctx, LOG_DEBUG,
				       "[pamh: %p] STATE: %s(%s) = %p",
				       ctx->pamh, type, key, data);
		}
	}
}