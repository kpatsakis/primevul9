gnutls_session_get_id (gnutls_session_t session,
                       void *session_id, size_t * session_id_size)
{
  size_t given_session_id_size = *session_id_size;

  *session_id_size = session->security_parameters.session_id_size;

  /* just return the session size */
  if (session_id == NULL)
    {
      return 0;
    }

  if (given_session_id_size < session->security_parameters.session_id_size)
    {
      return GNUTLS_E_SHORT_MEMORY_BUFFER;
    }

  memcpy (session_id, &session->security_parameters.session_id,
          *session_id_size);

  return 0;
}