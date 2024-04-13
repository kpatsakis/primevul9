int gnutls_session_is_resumed(gnutls_session_t session)
{
	if (session->security_parameters.entity == GNUTLS_CLIENT) {
		if (session->security_parameters.session_id_size > 0 &&
		    session->security_parameters.session_id_size ==
		    session->internals.resumed_security_parameters.
		    session_id_size
		    && memcmp(session->security_parameters.session_id,
			      session->
			      internals.resumed_security_parameters.
			      session_id,
			      session->security_parameters.
			      session_id_size) == 0)
			return 1;
	} else {
		if (session->internals.resumed != RESUME_FALSE)
			return 1;
	}

	return 0;
}