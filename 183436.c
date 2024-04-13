_gnutls_recv_supplemental (gnutls_session_t session)
{
  uint8_t *data = NULL;
  int datalen = 0;
  int ret;

  _gnutls_debug_log ("EXT[%x]: Expecting supplemental data\n", session);

  ret = _gnutls_recv_handshake (session, &data, &datalen,
				GNUTLS_HANDSHAKE_SUPPLEMENTAL,
				OPTIONAL_PACKET);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  ret = _gnutls_parse_supplemental (session, data, datalen);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  gnutls_free (data);

  return ret;
}