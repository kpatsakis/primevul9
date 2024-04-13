_gnutls_send_handshake (gnutls_session_t session, void *i_data,
			uint32_t i_datasize,
			gnutls_handshake_description_t type)
{
  int ret;
  uint8_t *data;
  uint32_t datasize;
  int pos = 0;

  if (i_data == NULL && i_datasize == 0)
    {
      /* we are resuming a previously interrupted
       * send.
       */
      ret = _gnutls_handshake_io_write_flush (session);
      return ret;

    }

  if (i_data == NULL && i_datasize > 0)
    {
      gnutls_assert ();
      return GNUTLS_E_INVALID_REQUEST;
    }

  /* first run */
  datasize = i_datasize + HANDSHAKE_HEADER_SIZE;
  data = gnutls_malloc (datasize);
  if (data == NULL)
    {
      gnutls_assert ();
      return GNUTLS_E_MEMORY_ERROR;
    }

  data[pos++] = (uint8_t) type;
  _gnutls_write_uint24 (i_datasize, &data[pos]);
  pos += 3;

  if (i_datasize > 0)
    memcpy (&data[pos], i_data, i_datasize);

  _gnutls_handshake_log ("HSK[%x]: %s was send [%ld bytes]\n",
			 session, _gnutls_handshake2str (type), datasize);


  /* Here we keep the handshake messages in order to hash them...
   */
  if (type != GNUTLS_HANDSHAKE_HELLO_REQUEST)
    if ((ret =
	 _gnutls_handshake_hash_add_sent (session, type, data, datasize)) < 0)
      {
	gnutls_assert ();
	gnutls_free (data);
	return ret;
      }

  session->internals.last_handshake_out = type;

  ret =
    _gnutls_handshake_io_send_int (session, GNUTLS_HANDSHAKE, type,
				   data, datasize);

  gnutls_free (data);

  return ret;
}