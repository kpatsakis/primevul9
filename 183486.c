_gnutls_send_supplemental (gnutls_session_t session, int again)
{
  int ret = 0;

  _gnutls_debug_log ("EXT[%x]: Sending supplemental data\n", session);

  if (again)
    ret = _gnutls_send_handshake (session, NULL, 0,
				  GNUTLS_HANDSHAKE_SUPPLEMENTAL);
  else
    {
      gnutls_buffer buf;
      _gnutls_buffer_init (&buf);

      ret = _gnutls_gen_supplemental (session, &buf);
      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}

      ret = _gnutls_send_handshake (session, buf.data, buf.length,
				    GNUTLS_HANDSHAKE_SUPPLEMENTAL);
      _gnutls_buffer_clear (&buf);
    }

  return ret;
}