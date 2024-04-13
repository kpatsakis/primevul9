static int oidc_handle_logout_backchannel(request_rec *r, oidc_cfg *cfg) {

	oidc_debug(r, "enter");

	const char *logout_token = NULL;
	oidc_jwt_t *jwt = NULL;
	oidc_jose_error_t err;
	oidc_jwk_t *jwk = NULL;
	oidc_provider_t *provider = NULL;
	char *sid = NULL, *uuid = NULL;
	oidc_session_t session;
	int rc = HTTP_BAD_REQUEST;

	apr_table_t *params = apr_table_make(r->pool, 8);
	if (oidc_util_read_post_params(r, params, FALSE, NULL) == FALSE) {
		oidc_error(r,
				"could not read POST-ed parameters to the logout endpoint");
		goto out;
	}

	logout_token = apr_table_get(params, OIDC_PROTO_LOGOUT_TOKEN);
	if (logout_token == NULL) {
		oidc_error(r,
				"backchannel lggout endpoint was called but could not find a parameter named \"%s\"",
				OIDC_PROTO_LOGOUT_TOKEN);
		goto out;
	}

	// TODO: jwk symmetric key based on provider

	if (oidc_jwt_parse(r->pool, logout_token, &jwt,
			oidc_util_merge_symmetric_key(r->pool, cfg->private_keys, NULL),
			&err) == FALSE) {
		oidc_error(r, "oidc_jwt_parse failed: %s", oidc_jose_e2s(r->pool, err));
		goto out;
	}

	if ((jwt->header.alg == NULL) || (strcmp(jwt->header.alg, "none") == 0)) {
		oidc_error(r, "logout token is not signed");
		goto out;
	}

	provider = oidc_get_provider_for_issuer(r, cfg, jwt->payload.iss, FALSE);
	if (provider == NULL) {
		oidc_error(r, "no provider found for issuer: %s", jwt->payload.iss);
		goto out;
	}

	// TODO: destroy the JWK used for decryption

	jwk = NULL;
	if (oidc_util_create_symmetric_key(r, provider->client_secret, 0,
			NULL, TRUE, &jwk) == FALSE)
		return FALSE;

	oidc_jwks_uri_t jwks_uri = { provider->jwks_uri,
			provider->jwks_refresh_interval, provider->ssl_validate_server };
	if (oidc_proto_jwt_verify(r, cfg, jwt, &jwks_uri,
			oidc_util_merge_symmetric_key(r->pool, NULL, jwk),
			provider->id_token_signed_response_alg) == FALSE) {

		oidc_error(r, "id_token signature could not be validated, aborting");
		goto out;
	}

	// oidc_proto_validate_idtoken would try and require a token binding cnf
	// if the policy is set to "required", so don't use that here
	if (oidc_proto_validate_jwt(r, jwt,
			provider->validate_issuer ? provider->issuer : NULL, FALSE, FALSE,
					provider->idtoken_iat_slack,
					OIDC_TOKEN_BINDING_POLICY_DISABLED) == FALSE)
		goto out;

	/* verify the "aud" and "azp" values */
	if (oidc_proto_validate_aud_and_azp(r, cfg, provider, &jwt->payload)
			== FALSE)
		goto out;

	json_t *events = json_object_get(jwt->payload.value.json,
			OIDC_CLAIM_EVENTS);
	if (events == NULL) {
		oidc_error(r, "\"%s\" claim could not be found in logout token",
				OIDC_CLAIM_EVENTS);
		goto out;
	}

	json_t *blogout = json_object_get(events, OIDC_EVENTS_BLOGOUT_KEY);
	if (!json_is_object(blogout)) {
		oidc_error(r, "\"%s\" object could not be found in \"%s\" claim",
				OIDC_EVENTS_BLOGOUT_KEY, OIDC_CLAIM_EVENTS);
		goto out;
	}

	char *nonce = NULL;
	oidc_json_object_get_string(r->pool, jwt->payload.value.json,
			OIDC_CLAIM_NONCE, &nonce, NULL);
	if (nonce != NULL) {
		oidc_error(r,
				"rejecting logout request/token since it contains a \"%s\" claim",
				OIDC_CLAIM_NONCE);
		goto out;
	}

	char *jti = NULL;
	oidc_json_object_get_string(r->pool, jwt->payload.value.json,
			OIDC_CLAIM_JTI, &jti, NULL);
	if (jti != NULL) {
		char *replay = NULL;
		oidc_cache_get_jti(r, jti, &replay);
		if (replay != NULL) {
			oidc_error(r,
					"the \"%s\" value (%s) passed in logout token was found in the cache already; possible replay attack!?",
					OIDC_CLAIM_JTI, jti);
			goto out;
		}
	}

	/* jti cache duration is the configured replay prevention window for token issuance plus 10 seconds for safety */
	apr_time_t jti_cache_duration = apr_time_from_sec(
			provider->idtoken_iat_slack * 2 + 10);

	/* store it in the cache for the calculated duration */
	oidc_cache_set_jti(r, jti, jti, apr_time_now() + jti_cache_duration);

	oidc_json_object_get_string(r->pool, jwt->payload.value.json,
			OIDC_CLAIM_EVENTS, &sid, NULL);

	// TODO: by-spec we should cater for the fact that "sid" has been provided
	//       in the id_token returned in the authentication request, but "sub"
	//       is used in the logout token but that requires a 2nd entry in the
	//       cache and a separate session "sub" member, ugh; we'll just assume
	//       that is "sid" is specified in the id_token, the OP will actually use
	//       this for logout
	//       (and probably call us multiple times or the same sub if needed)

	oidc_json_object_get_string(r->pool, jwt->payload.value.json,
			OIDC_CLAIM_SID, &sid, NULL);
	if (sid == NULL)
		sid = jwt->payload.sub;

	if (sid == NULL) {
		oidc_error(r, "no \"sub\" and no \"sid\" claim found in logout token");
		goto out;
	}

	// TODO: when dealing with sub instead of a true sid, we'll be killing all sessions for
	//       a specific user, across hosts that share the *same* cache backend
	//       if those hosts haven't been configured with a different OIDCCryptoPassphrase
	//       - perhaps that's even acceptable since non-memory caching is encrypted by default
	//         and memory-based caching doesn't suffer from this (different shm segments)?
	//       - it will result in 400 errors returned from backchannel logout calls to the other hosts...

	sid = oidc_make_sid_iss_unique(r, sid, provider->issuer);
	oidc_cache_get_sid(r, sid, &uuid);
	if (uuid == NULL) {
		// this may happen when we are the caller
		oidc_warn(r,
				"could not (or no longer) find a session based on sid/sub provided in logout token: %s",
				sid);
		r->user = "";
		rc = OK;
		goto out;
	}

	// revoke tokens if we can get a handle on those
	if (cfg->session_type != OIDC_SESSION_TYPE_CLIENT_COOKIE) {
		if (oidc_session_load_cache_by_uuid(r, cfg, uuid, &session) != FALSE)
			if (oidc_session_extract(r, &session) != FALSE)
				oidc_revoke_tokens(r, cfg, &session);
	}

	// clear the session cache
	oidc_cache_set_sid(r, sid, NULL, 0);
	oidc_cache_set_session(r, uuid, NULL, 0);

	r->user = "";
	rc = OK;

out:

	if (jwk != NULL) {
		oidc_jwk_destroy(jwk);
		jwk = NULL;

	}
	if (jwt != NULL) {
		oidc_jwt_destroy(jwt);
		jwt = NULL;
	}

	oidc_util_hdr_err_out_add(r, OIDC_HTTP_HDR_CACHE_CONTROL,
			"no-cache, no-store");
	oidc_util_hdr_err_out_add(r, OIDC_HTTP_HDR_PRAGMA, "no-cache");

	return rc;
}