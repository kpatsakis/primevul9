static const char *get_conf_item_string(struct pwb_context *ctx,
					const char *item,
					int config_flag)
{
	int i = 0;
	const char *parm_opt = NULL;

	if (!(ctx->ctrl & config_flag)) {
		goto out;
	}

	/* let the pam opt take precedence over the pam_winbind.conf option */
	for (i=0; i<ctx->argc; i++) {

		if ((strncmp(ctx->argv[i], item, strlen(item)) == 0)) {
			char *p;

			if ((p = strchr(ctx->argv[i], '=')) == NULL) {
				_pam_log(ctx, LOG_INFO,
					 "no \"=\" delimiter for \"%s\" found\n",
					 item);
				goto out;
			}
			_pam_log_debug(ctx, LOG_INFO,
				       "PAM config: %s '%s'\n", item, p+1);
			return p + 1;
		}
	}

	if (ctx->dict) {
		char *key = NULL;

		key = talloc_asprintf(ctx, "global:%s", item);
		if (!key) {
			goto out;
		}

		parm_opt = iniparser_getstring_nonempty(ctx->dict, key, NULL);
		TALLOC_FREE(key);

		_pam_log_debug(ctx, LOG_INFO, "CONFIG file: %s '%s'\n",
			       item, parm_opt);
	}
out:
	return parm_opt;
}