int oidc_handle_redirect_uri_request(request_rec *r, oidc_cfg *c,
		oidc_session_t *session) {

	if (oidc_proto_is_redirect_authorization_response(r, c)) {

		/* this is an authorization response from the OP using the Basic Client profile or a Hybrid flow*/
		return oidc_handle_redirect_authorization_response(r, c, session);
		/*
		 *
		 * Note that we are checking for logout *before* checking for a POST authorization response
		 * to handle backchannel POST-based logout
		 *
		 * so any POST to the Redirect URI that does not have a logout query parameter will be handled
		 * as an authorization response; alternatively we could assume that a POST response has no
		 * parameters
		 */
	} else if (oidc_util_request_has_parameter(r,
			OIDC_REDIRECT_URI_REQUEST_LOGOUT)) {
		/* handle logout */
		return oidc_handle_logout(r, c, session);

	} else if (oidc_proto_is_post_authorization_response(r, c)) {

		/* this is an authorization response using the fragment(+POST) response_mode with the Implicit Client profile */
		return oidc_handle_post_authorization_response(r, c, session);

	} else if (oidc_is_discovery_response(r, c)) {

		/* this is response from the OP discovery page */
		return oidc_handle_discovery_response(r, c);

	} else if (oidc_util_request_has_parameter(r,
			OIDC_REDIRECT_URI_REQUEST_JWKS)) {
		/*
		 * Will be handled in the content handler; avoid:
		 * No authentication done but request not allowed without authentication
		 * by setting r->user
		 */
		r->user = "";
		return OK;

	} else if (oidc_util_request_has_parameter(r,
			OIDC_REDIRECT_URI_REQUEST_SESSION)) {

		/* handle session management request */
		return oidc_handle_session_management(r, c, session);

	} else if (oidc_util_request_has_parameter(r,
			OIDC_REDIRECT_URI_REQUEST_REFRESH)) {

		/* handle refresh token request */
		return oidc_handle_refresh_token_request(r, c, session);

	} else if (oidc_util_request_has_parameter(r,
			OIDC_REDIRECT_URI_REQUEST_REQUEST_URI)) {

		/* handle request object by reference request */
		return oidc_handle_request_uri(r, c);

	} else if (oidc_util_request_has_parameter(r,
			OIDC_REDIRECT_URI_REQUEST_REMOVE_AT_CACHE)) {

		/* handle request to invalidate access token cache */
		return oidc_handle_remove_at_cache(r, c);

	} else if (oidc_util_request_has_parameter(r,
			OIDC_REDIRECT_URI_REQUEST_INFO)) {

		if (session->remote_user == NULL)
			return HTTP_UNAUTHORIZED;

		/*
		 * Will be handled in the content handler; avoid:
		 * No authentication done but request not allowed without authentication
		 * by setting r->user
		 */
		r->user = "";
		return OK;

	} else if ((r->args == NULL) || (apr_strnatcmp(r->args, "") == 0)) {

		/* this is a "bare" request to the redirect URI, indicating implicit flow using the fragment response_mode */
		return oidc_proto_javascript_implicit(r, c);
	}

	/* this is not an authorization response or logout request */

	/* check for "error" response */
	if (oidc_util_request_has_parameter(r, OIDC_PROTO_ERROR)) {

		//		char *error = NULL, *descr = NULL;
		//		oidc_util_get_request_parameter(r, "error", &error);
		//		oidc_util_get_request_parameter(r, "error_description", &descr);
		//
		//		/* send user facing error to browser */
		//		return oidc_util_html_send_error(r, error, descr, OK);
		return oidc_handle_redirect_authorization_response(r, c, session);
	}

	oidc_error(r,
			"The OpenID Connect callback URL received an invalid request: %s; returning HTTP_INTERNAL_SERVER_ERROR",
			r->args);

	/* something went wrong */
	return oidc_util_html_send_error(r, c->error_template, "Invalid Request",
			apr_psprintf(r->pool,
					"The OpenID Connect callback URL received an invalid request"),
					HTTP_INTERNAL_SERVER_ERROR);
}