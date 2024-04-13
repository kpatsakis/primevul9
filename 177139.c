static int oidc_handle_logout(request_rec *r, oidc_cfg *c,
		oidc_session_t *session) {

	oidc_provider_t *provider = NULL;
	/* pickup the command or URL where the user wants to go after logout */
	char *url = NULL;
	char *error_str = NULL;
	char *error_description = NULL;

	oidc_util_get_request_parameter(r, OIDC_REDIRECT_URI_REQUEST_LOGOUT, &url);

	oidc_debug(r, "enter (url=%s)", url);

	if (oidc_is_front_channel_logout(url)) {
		return oidc_handle_logout_request(r, c, session, url);
	} else if (oidc_is_back_channel_logout(url)) {
		return oidc_handle_logout_backchannel(r, c);
	}

	if ((url == NULL) || (apr_strnatcmp(url, "") == 0)) {

		url = c->default_slo_url;

	} else {

		/* do input validation on the logout parameter value */
		if (oidc_validate_redirect_url(r, c, url, TRUE, &error_str,
				&error_description) == FALSE) {
			return oidc_util_html_send_error(r, c->error_template, error_str,
					error_description,
					HTTP_BAD_REQUEST);
		}
	}

	oidc_get_provider_from_session(r, c, session, &provider);

	if ((provider != NULL) && (provider->end_session_endpoint != NULL)) {

		const char *id_token_hint = oidc_session_get_idtoken(r, session);

		char *logout_request = apr_pstrdup(r->pool,
				provider->end_session_endpoint);
		if (id_token_hint != NULL) {
			logout_request = apr_psprintf(r->pool, "%s%sid_token_hint=%s",
					logout_request, strchr(logout_request ? logout_request : "",
							OIDC_CHAR_QUERY) != NULL ?
									OIDC_STR_AMP :
									OIDC_STR_QUERY,
									oidc_util_escape_string(r, id_token_hint));
		}

		if (url != NULL) {
			logout_request = apr_psprintf(r->pool,
					"%s%spost_logout_redirect_uri=%s", logout_request,
					strchr(logout_request ? logout_request : "",
							OIDC_CHAR_QUERY) != NULL ?
									OIDC_STR_AMP :
									OIDC_STR_QUERY,
									oidc_util_escape_string(r, url));
		}
		//char *state = NULL;
		//oidc_proto_generate_nonce(r, &state, 8);
		//url = apr_psprintf(r->pool, "%s&state=%s", logout_request, state);
		url = logout_request;
	}

	return oidc_handle_logout_request(r, c, session, url);
}