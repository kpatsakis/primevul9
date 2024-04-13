static int oidc_handle_logout_request(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, const char *url) {

	oidc_debug(r, "enter (url=%s)", url);

	/* if there's no remote_user then there's no (stored) session to kill */
	if (session->remote_user != NULL)
		oidc_revoke_tokens(r, c, session);

	/*
	 * remove session state (cq. cache entry and cookie)
	 * always clear the session cookie because the cookie may be not sent (but still in the browser)
	 * due to SameSite policies
	 */
	oidc_session_kill(r, session);

	/* see if this is the OP calling us */
	if (oidc_is_front_channel_logout(url)) {

		/* set recommended cache control headers */
		oidc_util_hdr_err_out_add(r, OIDC_HTTP_HDR_CACHE_CONTROL,
				"no-cache, no-store");
		oidc_util_hdr_err_out_add(r, OIDC_HTTP_HDR_PRAGMA, "no-cache");
		oidc_util_hdr_err_out_add(r, OIDC_HTTP_HDR_P3P, "CAO PSA OUR");
		oidc_util_hdr_err_out_add(r, OIDC_HTTP_HDR_EXPIRES, "0");
		oidc_util_hdr_err_out_add(r, OIDC_HTTP_HDR_X_FRAME_OPTIONS, "DENY");

		/* see if this is PF-PA style logout in which case we return a transparent pixel */
		const char *accept = oidc_util_hdr_in_accept_get(r);
		if ((apr_strnatcmp(url, OIDC_IMG_STYLE_LOGOUT_PARAM_VALUE) == 0)
				|| ((accept) && strstr(accept, OIDC_CONTENT_TYPE_IMAGE_PNG))) {
			return oidc_util_http_send(r, (const char*) &oidc_transparent_pixel,
					sizeof(oidc_transparent_pixel), OIDC_CONTENT_TYPE_IMAGE_PNG,
					OK);
		}

		/* standard HTTP based logout: should be called in an iframe from the OP */
		return oidc_util_html_send(r, "Logged Out", NULL, NULL,
				"<p>Logged Out</p>", OK);
	}

	/* see if we don't need to go somewhere special after killing the session locally */
	if (url == NULL)
		return oidc_util_html_send(r, "Logged Out", NULL, NULL,
				"<p>Logged Out</p>", OK);

	/* send the user to the specified where-to-go-after-logout URL */
	oidc_util_hdr_out_location_set(r, url);

	return HTTP_MOVED_TEMPORARILY;
}