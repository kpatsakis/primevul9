_gnutls_client_check_if_resuming (gnutls_session_t session,
				  opaque * session_id, int session_id_len)
{
  opaque buf[2 * TLS_MAX_SESSION_ID_SIZE + 1];

  _gnutls_handshake_log ("HSK[%x]: SessionID length: %d\n", session,
			 session_id_len);
  _gnutls_handshake_log ("HSK[%x]: SessionID: %s\n", session,
			 _gnutls_bin2hex (session_id, session_id_len, buf,
					  sizeof (buf)));

  if (session_id_len > 0 &&
      session->internals.resumed_security_parameters.session_id_size ==
      session_id_len
      && memcmp (session_id,
		 session->internals.resumed_security_parameters.
		 session_id, session_id_len) == 0)
    {
      /* resume session */
      memcpy (session->internals.
	      resumed_security_parameters.server_random,
	      session->security_parameters.server_random, TLS_RANDOM_SIZE);
      memcpy (session->internals.
	      resumed_security_parameters.client_random,
	      session->security_parameters.client_random, TLS_RANDOM_SIZE);
      session->internals.resumed = RESUME_TRUE;	/* we are resuming */

      return 0;
    }
  else
    {
      /* keep the new session id */
      session->internals.resumed = RESUME_FALSE;	/* we are not resuming */
      session->security_parameters.session_id_size = session_id_len;
      memcpy (session->security_parameters.session_id,
	      session_id, session_id_len);

      return -1;
    }
}