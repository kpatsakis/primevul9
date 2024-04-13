_gnutls_recv_handshake (gnutls_session_t session, uint8_t ** data,
			int *datalen, gnutls_handshake_description_t type,
			Optional optional)
{
  int ret;
  uint32_t length32 = 0;
  opaque *dataptr = NULL;
  gnutls_handshake_description_t recv_type;

  ret = _gnutls_recv_handshake_header (session, type, &recv_type);
  if (ret < 0)
    {

      if (ret == GNUTLS_E_UNEXPECTED_HANDSHAKE_PACKET
	  && optional == OPTIONAL_PACKET)
	{
	  if (datalen != NULL)
	    *datalen = 0;
	  if (data != NULL)
	    *data = NULL;
	  return 0;		/* ok just ignore the packet */
	}

      return ret;
    }

  session->internals.last_handshake_in = recv_type;

  length32 = ret;

  if (length32 > 0)
    dataptr = gnutls_malloc (length32);
  else if (recv_type != GNUTLS_HANDSHAKE_SERVER_HELLO_DONE)
    {
      gnutls_assert ();
      return GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
    }

  if (dataptr == NULL && length32 > 0)
    {
      gnutls_assert ();
      return GNUTLS_E_MEMORY_ERROR;
    }

  if (datalen != NULL)
    *datalen = length32;

  if (length32 > 0)
    {
      ret =
	_gnutls_handshake_io_recv_int (session, GNUTLS_HANDSHAKE,
				       type, dataptr, length32);
      if (ret <= 0)
	{
	  gnutls_assert ();
	  gnutls_free (dataptr);
	  return (ret == 0) ? GNUTLS_E_UNEXPECTED_PACKET_LENGTH : ret;
	}
    }

  if (data != NULL && length32 > 0)
    *data = dataptr;


  ret = _gnutls_handshake_hash_add_recvd (session, recv_type,
					  session->internals.
					  handshake_header_buffer.header,
					  session->internals.
					  handshake_header_buffer.
					  header_size, dataptr, length32);
  if (ret < 0)
    {
      gnutls_assert ();
      _gnutls_handshake_header_buffer_clear (session);
      return ret;
    }

  /* If we fail before this then we will reuse the handshake header
   * have have received above. if we get here the we clear the handshake
   * header we received.
   */
  _gnutls_handshake_header_buffer_clear (session);

  switch (recv_type)
    {
    case GNUTLS_HANDSHAKE_CLIENT_HELLO:
    case GNUTLS_HANDSHAKE_SERVER_HELLO:
      ret = _gnutls_recv_hello (session, dataptr, length32);
      /* dataptr is freed because the caller does not
       * need it */
      gnutls_free (dataptr);
      if (data != NULL)
	*data = NULL;
      break;
    case GNUTLS_HANDSHAKE_SERVER_HELLO_DONE:
      if (length32 == 0)
	ret = 0;
      else
	ret = GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
      break;
    case GNUTLS_HANDSHAKE_CERTIFICATE_PKT:
    case GNUTLS_HANDSHAKE_FINISHED:
    case GNUTLS_HANDSHAKE_SERVER_KEY_EXCHANGE:
    case GNUTLS_HANDSHAKE_CLIENT_KEY_EXCHANGE:
    case GNUTLS_HANDSHAKE_CERTIFICATE_REQUEST:
    case GNUTLS_HANDSHAKE_CERTIFICATE_VERIFY:
    case GNUTLS_HANDSHAKE_SUPPLEMENTAL:
      ret = length32;
      break;
    default:
      gnutls_assert ();
      gnutls_free (dataptr);
      if (data != NULL)
	*data = NULL;
      ret = GNUTLS_E_UNEXPECTED_HANDSHAKE_PACKET;
    }

  return ret;
}