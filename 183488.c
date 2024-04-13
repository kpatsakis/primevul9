gnutls_server_name_set (gnutls_session_t session,
			gnutls_server_name_type_t type,
			const void *name, size_t name_length)
{
  int server_names;

  if (session->security_parameters.entity == GNUTLS_SERVER)
    {
      gnutls_assert ();
      return GNUTLS_E_INVALID_REQUEST;
    }

  if (name_length > MAX_SERVER_NAME_SIZE)
    return GNUTLS_E_SHORT_MEMORY_BUFFER;

  server_names =
    session->security_parameters.extensions.server_names_size + 1;

  if (server_names > MAX_SERVER_NAME_EXTENSIONS)
    server_names = MAX_SERVER_NAME_EXTENSIONS;

  session->security_parameters.extensions.server_names[server_names -
						       1].type = type;
  memcpy (session->security_parameters.extensions.
	  server_names[server_names - 1].name, name, name_length);
  session->security_parameters.extensions.server_names[server_names -
						       1].name_length =
    name_length;

  session->security_parameters.extensions.server_names_size++;

  return 0;
}