static void oidc_revoke_tokens(request_rec *r, oidc_cfg *c,
		oidc_session_t *session) {

	char *response = NULL;
	char *basic_auth = NULL;
	char *bearer_auth = NULL;
	apr_table_t *params = NULL;
	const char *token = NULL;
	oidc_provider_t *provider = NULL;

	oidc_debug(r, "enter");

	if (oidc_get_provider_from_session(r, c, session, &provider) == FALSE)
		goto out;

	oidc_debug(r, "revocation_endpoint=%s",
			provider->revocation_endpoint_url ?
					provider->revocation_endpoint_url : "(null)");

	if (provider->revocation_endpoint_url == NULL)
		goto out;

	params = apr_table_make(r->pool, 4);

	// add the token endpoint authentication credentials to the revocation endpoint call...
	if (oidc_proto_token_endpoint_auth(r, c, provider->token_endpoint_auth,
			provider->client_id, provider->client_secret,
			provider->client_signing_keys, provider->token_endpoint_url, params,
			NULL, &basic_auth, &bearer_auth) == FALSE)
		goto out;

	// TODO: use oauth.ssl_validate_server ...
	token = oidc_session_get_refresh_token(r, session);
	if (token != NULL) {
		apr_table_setn(params, OIDC_PROTO_TOKEN_TYPE_HINT, OIDC_PROTO_REFRESH_TOKEN);
		apr_table_setn(params, OIDC_PROTO_TOKEN, token);

		if (oidc_util_http_post_form(r, provider->revocation_endpoint_url,
				params, basic_auth, bearer_auth, c->oauth.ssl_validate_server,
				&response, c->http_timeout_long, c->outgoing_proxy,
				oidc_dir_cfg_pass_cookies(r), NULL,
				NULL, NULL) == FALSE) {
			oidc_warn(r, "revoking refresh token failed");
		}
		apr_table_unset(params, OIDC_PROTO_TOKEN_TYPE_HINT);
		apr_table_unset(params, OIDC_PROTO_TOKEN);
	}

	token = oidc_session_get_access_token(r, session);
	if (token != NULL) {
		apr_table_setn(params, OIDC_PROTO_TOKEN_TYPE_HINT, OIDC_PROTO_ACCESS_TOKEN);
		apr_table_setn(params, OIDC_PROTO_TOKEN, token);

		if (oidc_util_http_post_form(r, provider->revocation_endpoint_url,
				params, basic_auth, bearer_auth, c->oauth.ssl_validate_server,
				&response, c->http_timeout_long, c->outgoing_proxy,
				oidc_dir_cfg_pass_cookies(r), NULL,
				NULL, NULL) == FALSE) {
			oidc_warn(r, "revoking access token failed");
		}
	}

out:

	oidc_debug(r, "leave");
}