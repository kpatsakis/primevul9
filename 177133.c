int oidc_content_handler(request_rec *r) {
	oidc_cfg *c = ap_get_module_config(r->server->module_config,
			&auth_openidc_module);
	int rc = DECLINED;
	/* track if the session needs to be updated/saved into the cache */
	apr_byte_t needs_save = FALSE;
	oidc_session_t *session = NULL;

	if (oidc_enabled(r) == TRUE) {

		if (oidc_util_request_matches_url(r, oidc_get_redirect_uri(r, c)) == TRUE) {

			if (oidc_util_request_has_parameter(r,
					OIDC_REDIRECT_URI_REQUEST_INFO)) {

				oidc_session_load(r, &session);

				rc = oidc_handle_existing_session(r, c, session, &needs_save);
				if (rc == OK)
					/* handle request for session info */
					rc = oidc_handle_info_request(r, c, session, needs_save);

				/* free resources allocated for the session */
				oidc_session_free(r, session);

			} else if (oidc_util_request_has_parameter(r,
					OIDC_REDIRECT_URI_REQUEST_JWKS)) {

				/* handle JWKs request */
				rc = oidc_handle_jwks(r, c);

			} else {

				rc = OK;

			}

		} else if (oidc_request_state_get(r, OIDC_REQUEST_STATE_KEY_DISCOVERY) != NULL) {

			rc = oidc_discovery(r, c);

		} else if (oidc_request_state_get(r, OIDC_REQUEST_STATE_KEY_AUTHN) != NULL) {

			rc = OK;

		}

	}

	return rc;
}