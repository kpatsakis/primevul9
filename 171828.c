static int _winbind_read_password(struct pwb_context *ctx,
				  unsigned int ctrl,
				  const char *comment,
				  const char *prompt1,
				  const char *prompt2,
				  const char **pass)
{
	int authtok_flag;
	int retval;
	const char *item;
	char *token;

	_pam_log(ctx, LOG_DEBUG, "getting password (0x%08x)", ctrl);

	/*
	 * make sure nothing inappropriate gets returned
	 */

	*pass = token = NULL;

	/*
	 * which authentication token are we getting?
	 */

	if (on(WINBIND__OLD_PASSWORD, ctrl)) {
		authtok_flag = PAM_OLDAUTHTOK;
	} else {
		authtok_flag = PAM_AUTHTOK;
	}

	/*
	 * should we obtain the password from a PAM item ?
	 */

	if (on(WINBIND_TRY_FIRST_PASS_ARG, ctrl) ||
	    on(WINBIND_USE_FIRST_PASS_ARG, ctrl)) {
		retval = _pam_get_item(ctx->pamh, authtok_flag, &item);
		if (retval != PAM_SUCCESS) {
			/* very strange. */
			_pam_log(ctx, LOG_ALERT,
				 "pam_get_item returned error "
				 "to unix-read-password");
			return retval;
		} else if (item != NULL) {	/* we have a password! */
			*pass = item;
			item = NULL;
			_pam_log(ctx, LOG_DEBUG,
				 "pam_get_item returned a password");
			return PAM_SUCCESS;
		} else if (on(WINBIND_USE_FIRST_PASS_ARG, ctrl)) {
			return PAM_AUTHTOK_RECOVER_ERR;	/* didn't work */
		} else if (on(WINBIND_USE_AUTHTOK_ARG, ctrl)
			   && off(WINBIND__OLD_PASSWORD, ctrl)) {
			return PAM_AUTHTOK_RECOVER_ERR;
		}
	}
	/*
	 * getting here implies we will have to get the password from the
	 * user directly.
	 */

	{
		struct pam_message msg[3], *pmsg[3];
		struct pam_response *resp;
		int i, replies;

		/* prepare to converse */

		if (comment != NULL && off(ctrl, WINBIND_SILENT)) {
			pmsg[0] = &msg[0];
			msg[0].msg_style = PAM_TEXT_INFO;
			msg[0].msg = discard_const_p(char, comment);
			i = 1;
		} else {
			i = 0;
		}

		pmsg[i] = &msg[i];
		msg[i].msg_style = PAM_PROMPT_ECHO_OFF;
		msg[i++].msg = discard_const_p(char, prompt1);
		replies = 1;

		if (prompt2 != NULL) {
			pmsg[i] = &msg[i];
			msg[i].msg_style = PAM_PROMPT_ECHO_OFF;
			msg[i++].msg = discard_const_p(char, prompt2);
			++replies;
		}
		/* so call the conversation expecting i responses */
		resp = NULL;
		retval = converse(ctx->pamh, i, pmsg, &resp);
		if (resp == NULL) {
			if (retval == PAM_SUCCESS) {
				retval = PAM_AUTHTOK_RECOVER_ERR;
			}
			goto done;
		}
		if (retval != PAM_SUCCESS) {
			_pam_drop_reply(resp, i);
			goto done;
		}

		/* interpret the response */

		token = x_strdup(resp[i - replies].resp);
		if (!token) {
			_pam_log(ctx, LOG_NOTICE,
				 "could not recover "
				 "authentication token");
			retval = PAM_AUTHTOK_RECOVER_ERR;
			goto done;
		}

		if (replies == 2) {
			/* verify that password entered correctly */
			if (!resp[i - 1].resp ||
			    strcmp(token, resp[i - 1].resp)) {
				_pam_delete(token);	/* mistyped */
				retval = PAM_AUTHTOK_RECOVER_ERR;
				_make_remark(ctx, PAM_ERROR_MSG,
					     MISTYPED_PASS);
			}
		}

		/*
		 * tidy up the conversation (resp_retcode) is ignored
		 * -- what is it for anyway? AGM
		 */
		_pam_drop_reply(resp, i);
	}

 done:
	if (retval != PAM_SUCCESS) {
		_pam_log_debug(ctx, LOG_DEBUG,
			       "unable to obtain a password");
		return retval;
	}
	/* 'token' is the entered password */

	/* we store this password as an item */

	retval = pam_set_item(ctx->pamh, authtok_flag, token);
	_pam_delete(token);	/* clean it up */
	if (retval != PAM_SUCCESS ||
	    (retval = _pam_get_item(ctx->pamh, authtok_flag, &item)) != PAM_SUCCESS) {

		_pam_log(ctx, LOG_CRIT, "error manipulating password");
		return retval;

	}

	*pass = item;
	item = NULL;		/* break link to password */

	return PAM_SUCCESS;
}