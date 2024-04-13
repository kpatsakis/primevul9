_gnutls_handshake_hash_add_recvd (gnutls_session_t session,
				  gnutls_handshake_description_t recv_type,
				  opaque * header, uint16_t header_size,
				  opaque * dataptr, uint32_t datalen)
{
  int ret;

  /* The idea here is to hash the previous message we received,
   * and add the one we just received into the handshake_hash_buffer.
   */

  if ((ret = _gnutls_handshake_hash_pending (session)) < 0)
    {
      gnutls_assert ();
      return ret;
    }

  /* here we buffer the handshake messages - needed at Finished message */
  if (recv_type != GNUTLS_HANDSHAKE_HELLO_REQUEST)
    {

      if ((ret =
	   _gnutls_handshake_buffer_put (session, header, header_size)) < 0)
	{
	  gnutls_assert ();
	  return ret;
	}

      if (datalen > 0)
	{
	  if ((ret =
	       _gnutls_handshake_buffer_put (session, dataptr, datalen)) < 0)
	    {
	      gnutls_assert ();
	      return ret;
	    }
	}
    }

  return 0;
}