_gnutls_generate_session_id (opaque * session_id, uint8_t * len)
{
  *len = TLS_MAX_SESSION_ID_SIZE;
  int ret;

  ret = _gnutls_rnd (RND_NONCE, session_id, *len);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  return 0;
}