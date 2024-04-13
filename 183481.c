gnutls_server_name_get (gnutls_session_t session, void *data,
			size_t * data_length,
			unsigned int *type, unsigned int indx)
{
  char *_data = data;

  if (session->security_parameters.entity == GNUTLS_CLIENT)
    {
      gnutls_assert ();
      return GNUTLS_E_INVALID_REQUEST;
    }

  if (indx + 1 > session->security_parameters.extensions.server_names_size)
    {
      return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
    }

  *type = session->security_parameters.extensions.server_names[indx].type;

  if (*data_length >		/* greater since we need one extra byte for the null */
      session->security_parameters.extensions.server_names[indx].name_length)
    {
      *data_length =
	session->security_parameters.extensions.server_names[indx].
	name_length;
      memcpy (data,
	      session->security_parameters.extensions.server_names[indx].
	      name, *data_length);

      if (*type == GNUTLS_NAME_DNS)	/* null terminate */
	_data[(*data_length)] = 0;

    }
  else
    {
      *data_length =
	session->security_parameters.extensions.server_names[indx].
	name_length;
      return GNUTLS_E_SHORT_MEMORY_BUFFER;
    }

  return 0;
}