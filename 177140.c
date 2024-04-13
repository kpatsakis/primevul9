int oidc_auth_checker(request_rec *r) {

	oidc_cfg *c = ap_get_module_config(r->server->module_config,
			&auth_openidc_module);

	/* check for anonymous access and PASS mode */
	if (r->user != NULL && strlen(r->user) == 0) {
		r->user = NULL;
		if (oidc_dir_cfg_unauth_action(r) == OIDC_UNAUTH_PASS)
			return OK;
		if if (oidc_request_state_get(r, OIDC_REQUEST_STATE_KEY_DISCOVERY) != NULL)
			return OK;
	}

	/* get the set of claims from the request state (they've been set in the authentication part earlier */
	json_t *claims = NULL, *id_token = NULL;
	oidc_authz_get_claims_and_idtoken(r, &claims, &id_token);

	/* get the Require statements */
	const apr_array_header_t * const reqs_arr = ap_requires(r);

	/* see if we have any */
	const require_line * const reqs =
			reqs_arr ? (require_line *) reqs_arr->elts : NULL;
	if (!reqs_arr) {
		oidc_debug(r,
				"no require statements found, so declining to perform authorization.");
		return DECLINED;
	}

	/* merge id_token claims (e.g. "iss") in to claims json object */
	if (claims)
		oidc_util_json_merge(r, id_token, claims);

	/* dispatch to the <2.4 specific authz routine */
	int rc = oidc_authz_worker22(r, claims ? claims : id_token, reqs,
			reqs_arr->nelts);

	/* cleanup */
	if (claims)
		json_decref(claims);
	if (id_token)
		json_decref(id_token);

	if ((rc == HTTP_UNAUTHORIZED) && ap_auth_type(r))
		rc = oidc_handle_unauthorized_user22(r);

	return rc;
}