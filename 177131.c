static int oidc_authenticate_user(request_rec *r, oidc_cfg *c,
		oidc_provider_t *provider, const char *original_url,
		const char *login_hint, const char *id_token_hint, const char *prompt,
		const char *auth_request_params, const char *path_scope) {

	oidc_debug(r, "enter");

	if (provider == NULL) {

		// TODO: should we use an explicit redirect to the discovery endpoint (maybe a "discovery" param to the redirect_uri)?
		if (c->metadata_dir != NULL) {
			/*
			 * Will be handled in the content handler; avoid:
			 * No authentication done but request not allowed without authentication
			 * by setting r->user
			 */
			oidc_debug(r, "defer discovery to the content handler");
			oidc_request_state_set(r, OIDC_REQUEST_STATE_KEY_DISCOVERY, "");
			r->user = "";
			return OK;
		}

		/* we're not using multiple OP's configured in a metadata directory, pick the statically configured OP */
		if (oidc_provider_static_config(r, c, &provider) == FALSE)
			return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* generate the random nonce value that correlates requests and responses */
	char *nonce = NULL;
	if (oidc_proto_generate_nonce(r, &nonce, OIDC_PROTO_NONCE_LENGTH) == FALSE)
		return HTTP_INTERNAL_SERVER_ERROR;

	char *pkce_state = NULL;
	char *code_challenge = NULL;

	if ((oidc_util_spaced_string_contains(r->pool, provider->response_type,
			OIDC_PROTO_CODE) == TRUE) && (provider->pkce != NULL)) {

		/* generate the code verifier value that correlates authorization requests and code exchange requests */
		if (provider->pkce->state(r, &pkce_state) == FALSE)
			return HTTP_INTERNAL_SERVER_ERROR;

		/* generate the PKCE code challenge */
		if (provider->pkce->challenge(r, pkce_state, &code_challenge) == FALSE)
			return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* create the state between request/response */
	oidc_proto_state_t *proto_state = oidc_proto_state_new();
	oidc_proto_state_set_original_url(proto_state, original_url);
	oidc_proto_state_set_original_method(proto_state,
			oidc_original_request_method(r, c, TRUE));
	oidc_proto_state_set_issuer(proto_state, provider->issuer);
	oidc_proto_state_set_response_type(proto_state, provider->response_type);
	oidc_proto_state_set_nonce(proto_state, nonce);
	oidc_proto_state_set_timestamp_now(proto_state);
	if (provider->response_mode)
		oidc_proto_state_set_response_mode(proto_state,
				provider->response_mode);
	if (prompt)
		oidc_proto_state_set_prompt(proto_state, prompt);
	if (pkce_state)
		oidc_proto_state_set_pkce_state(proto_state, pkce_state);

	/* get a hash value that fingerprints the browser concatenated with the random input */
	char *state = oidc_get_browser_state_hash(r, c, nonce);

	/*
	 * create state that restores the context when the authorization response comes in
	 * and cryptographically bind it to the browser
	 */
	int rc = oidc_authorization_request_set_cookie(r, c, state, proto_state);
	if (rc != OK) {
		oidc_proto_state_destroy(proto_state);
		return rc;
	}

	/*
	 * printout errors if Cookie settings are not going to work
	 * TODO: separate this code out into its own function
	 */
	apr_uri_t o_uri;
	memset(&o_uri, 0, sizeof(apr_uri_t));
	apr_uri_t r_uri;
	memset(&r_uri, 0, sizeof(apr_uri_t));
	apr_uri_parse(r->pool, original_url, &o_uri);
	apr_uri_parse(r->pool, oidc_get_redirect_uri(r, c), &r_uri);
	if ((apr_strnatcmp(o_uri.scheme, r_uri.scheme) != 0)
			&& (apr_strnatcmp(r_uri.scheme, "https") == 0)) {
		oidc_error(r,
				"the URL scheme (%s) of the configured " OIDCRedirectURI " does not match the URL scheme of the URL being accessed (%s): the \"state\" and \"session\" cookies will not be shared between the two!",
				r_uri.scheme, o_uri.scheme);
		oidc_proto_state_destroy(proto_state);
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	if (c->cookie_domain == NULL) {
		if (apr_strnatcmp(o_uri.hostname, r_uri.hostname) != 0) {
			char *p = strstr(o_uri.hostname, r_uri.hostname);
			if ((p == NULL) || (apr_strnatcmp(r_uri.hostname, p) != 0)) {
				oidc_error(r,
						"the URL hostname (%s) of the configured " OIDCRedirectURI " does not match the URL hostname of the URL being accessed (%s): the \"state\" and \"session\" cookies will not be shared between the two!",
						r_uri.hostname, o_uri.hostname);
				oidc_proto_state_destroy(proto_state);
				return HTTP_INTERNAL_SERVER_ERROR;
			}
		}
	} else {
		if (!oidc_util_cookie_domain_valid(r_uri.hostname, c->cookie_domain)) {
			oidc_error(r,
					"the domain (%s) configured in " OIDCCookieDomain " does not match the URL hostname (%s) of the URL being accessed (%s): setting \"state\" and \"session\" cookies will not work!!",
					c->cookie_domain, o_uri.hostname, original_url);
			oidc_proto_state_destroy(proto_state);
			return HTTP_INTERNAL_SERVER_ERROR;
		}
	}

	/* send off to the OpenID Connect Provider */
	// TODO: maybe show intermediate/progress screen "redirecting to"
	return oidc_proto_authorization_request(r, provider, login_hint,
			oidc_get_redirect_uri_iss(r, c, provider), state, proto_state,
			id_token_hint, code_challenge, auth_request_params, path_scope);
}