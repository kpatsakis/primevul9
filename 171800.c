static int get_config_item_int(struct pwb_context *ctx,
			       const char *item,
			       int config_flag)
{
	int i, parm_opt = -1;

	if (!(ctx->ctrl & config_flag)) {
		goto out;
	}

	/* let the pam opt take precedence over the pam_winbind.conf option */
	for (i = 0; i < ctx->argc; i++) {

		if ((strncmp(ctx->argv[i], item, strlen(item)) == 0)) {
			char *p;

			if ((p = strchr(ctx->argv[i], '=')) == NULL) {
				_pam_log(ctx, LOG_INFO,
					 "no \"=\" delimiter for \"%s\" found\n",
					 item);
				goto out;
			}
			parm_opt = atoi(p + 1);
			_pam_log_debug(ctx, LOG_INFO,
				       "PAM config: %s '%d'\n",
				       item, parm_opt);
			return parm_opt;
		}
	}

	if (ctx->dict) {
		char *key = NULL;

		key = talloc_asprintf(ctx, "global:%s", item);
		if (!key) {
			goto out;
		}

		parm_opt = iniparser_getint(ctx->dict, key, -1);
		TALLOC_FREE(key);

		_pam_log_debug(ctx, LOG_INFO,
			       "CONFIG file: %s '%d'\n",
			       item, parm_opt);
	}
out:
	return parm_opt;
}