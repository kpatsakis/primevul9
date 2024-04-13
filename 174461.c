cssp_connect(char *server, char *user, char *domain, char *password, STREAM s)
{
	OM_uint32 actual_time;
	gss_cred_id_t cred;
	gss_buffer_desc input_tok, output_tok;
	gss_name_t target_name;
	OM_uint32 major_status, minor_status;
	int context_established = 0;
	gss_ctx_id_t gss_ctx;
	gss_OID desired_mech = &_gss_spnego_krb5_mechanism_oid_desc;

	STREAM ts_creds;
	struct stream token = { 0 };
	struct stream pubkey = { 0 };
	struct stream pubkey_cmp = { 0 };

	// Verify that system gss support spnego
	if (!cssp_gss_mech_available(desired_mech))
	{
		warning("CredSSP: System doesn't have support for desired authentication mechanism.\n");
		return False;
	}

	// Get service name
	if (!cssp_gss_get_service_name(server, &target_name))
	{
		warning("CredSSP: Failed to get target service name.\n");
		return False;
	}

	// Establish tls connection to server
	if (!tcp_tls_connect())
	{
		warning("CredSSP: Failed to establish TLS connection.\n");
		return False;
	}

	tcp_tls_get_server_pubkey(&pubkey);

#ifdef WITH_DEBUG_CREDSSP
	streamsave(&pubkey, "PubKey.raw");
#endif

	// Enter the spnego loop
	OM_uint32 actual_services;
	gss_OID actual_mech;
	struct stream blob = { 0 };

	gss_ctx = GSS_C_NO_CONTEXT;
	cred = GSS_C_NO_CREDENTIAL;

	input_tok.length = 0;
	output_tok.length = 0;
	minor_status = 0;

	int i = 0;

	do
	{
		major_status = gss_init_sec_context(&minor_status,
						    cred,
						    &gss_ctx,
						    target_name,
						    desired_mech,
						    GSS_C_MUTUAL_FLAG | GSS_C_DELEG_FLAG,
						    GSS_C_INDEFINITE,
						    GSS_C_NO_CHANNEL_BINDINGS,
						    &input_tok,
						    &actual_mech,
						    &output_tok, &actual_services, &actual_time);

		if (GSS_ERROR(major_status))
		{
			if (i == 0)
				error("CredSSP: Initialize failed, do you have correct kerberos tgt initialized ?\n");
			else
				error("CredSSP: Negotiation failed.\n");

#ifdef WITH_DEBUG_CREDSSP
			cssp_gss_report_error(GSS_C_GSS_CODE, "CredSSP: SPNEGO negotiation failed.",
					      major_status, minor_status);
#endif
			goto bail_out;
		}

		// validate required services
		if (!(actual_services & GSS_C_CONF_FLAG))
		{
			error("CredSSP: Confidiality service required but is not available.\n");
			goto bail_out;
		}

		// Send token to server
		if (output_tok.length != 0)
		{
			if (output_tok.length > token.size)
				s_realloc(&token, output_tok.length);
			s_reset(&token);

			out_uint8p(&token, output_tok.value, output_tok.length);
			s_mark_end(&token);

			if (!cssp_send_tsrequest(&token, NULL, NULL))
				goto bail_out;

			(void) gss_release_buffer(&minor_status, &output_tok);
		}

		// Read token from server
		if (major_status & GSS_S_CONTINUE_NEEDED)
		{
			(void) gss_release_buffer(&minor_status, &input_tok);

			if (!cssp_read_tsrequest(&token, NULL))
				goto bail_out;

			input_tok.value = token.data;
			input_tok.length = s_length(&token);
		}
		else
		{
			// Send encrypted pubkey for verification to server
			context_established = 1;

			if (!cssp_gss_wrap(gss_ctx, &pubkey, &blob))
				goto bail_out;

			if (!cssp_send_tsrequest(NULL, NULL, &blob))
				goto bail_out;

			context_established = 1;
		}

		i++;

	}
	while (!context_established);

	// read tsrequest response and decrypt for public key validation
	if (!cssp_read_tsrequest(NULL, &blob))
		goto bail_out;

	if (!cssp_gss_unwrap(gss_ctx, &blob, &pubkey_cmp))
		goto bail_out;

	pubkey_cmp.data[0] -= 1;

	// validate public key
	if (memcmp(pubkey.data, pubkey_cmp.data, s_length(&pubkey)) != 0)
	{
		error("CredSSP: Cannot guarantee integrity of server connection, MITM ? "
		      "(public key data mismatch)\n");
		goto bail_out;
	}

	// Send TSCredentials
	ts_creds = cssp_encode_tscredentials(user, password, domain);

	if (!cssp_gss_wrap(gss_ctx, ts_creds, &blob))
		goto bail_out;

	s_free(ts_creds);

	if (!cssp_send_tsrequest(NULL, &blob, NULL))
		goto bail_out;

	return True;

      bail_out:
	xfree(token.data);
	return False;
}