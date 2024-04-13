static int datablob_parse(char *datablob, const char **format,
			  char **master_desc, char **decrypted_datalen,
			  char **hex_encoded_iv)
{
	substring_t args[MAX_OPT_ARGS];
	int ret = -EINVAL;
	int key_cmd;
	int key_format;
	char *p, *keyword;

	keyword = strsep(&datablob, " \t");
	if (!keyword) {
		pr_info("encrypted_key: insufficient parameters specified\n");
		return ret;
	}
	key_cmd = match_token(keyword, key_tokens, args);

	/* Get optional format: default | ecryptfs */
	p = strsep(&datablob, " \t");
	if (!p) {
		pr_err("encrypted_key: insufficient parameters specified\n");
		return ret;
	}

	key_format = match_token(p, key_format_tokens, args);
	switch (key_format) {
	case Opt_ecryptfs:
	case Opt_default:
		*format = p;
		*master_desc = strsep(&datablob, " \t");
		break;
	case Opt_error:
		*master_desc = p;
		break;
	}

	if (!*master_desc) {
		pr_info("encrypted_key: master key parameter is missing\n");
		goto out;
	}

	if (valid_master_desc(*master_desc, NULL) < 0) {
		pr_info("encrypted_key: master key parameter \'%s\' "
			"is invalid\n", *master_desc);
		goto out;
	}

	if (decrypted_datalen) {
		*decrypted_datalen = strsep(&datablob, " \t");
		if (!*decrypted_datalen) {
			pr_info("encrypted_key: keylen parameter is missing\n");
			goto out;
		}
	}

	switch (key_cmd) {
	case Opt_new:
		if (!decrypted_datalen) {
			pr_info("encrypted_key: keyword \'%s\' not allowed "
				"when called from .update method\n", keyword);
			break;
		}
		ret = 0;
		break;
	case Opt_load:
		if (!decrypted_datalen) {
			pr_info("encrypted_key: keyword \'%s\' not allowed "
				"when called from .update method\n", keyword);
			break;
		}
		*hex_encoded_iv = strsep(&datablob, " \t");
		if (!*hex_encoded_iv) {
			pr_info("encrypted_key: hex blob is missing\n");
			break;
		}
		ret = 0;
		break;
	case Opt_update:
		if (decrypted_datalen) {
			pr_info("encrypted_key: keyword \'%s\' not allowed "
				"when called from .instantiate method\n",
				keyword);
			break;
		}
		ret = 0;
		break;
	case Opt_err:
		pr_info("encrypted_key: keyword \'%s\' not recognized\n",
			keyword);
		break;
	}
out:
	return ret;
}