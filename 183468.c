int _gnutls_user_hello_func( gnutls_session session, gnutls_protocol_t adv_version)
{
int ret;

  if (session->internals.user_hello_func != NULL) 
    {
      ret = session->internals.user_hello_func( session);
      if (ret < 0) 
        {
          gnutls_assert();
          return ret;
        }
      /* Here we need to renegotiate the version since the callee might
       * have disabled some TLS versions.
       */
      ret = _gnutls_negotiate_version( session, adv_version);
      if (ret < 0) {
        gnutls_assert();
        return ret;
      }
    }
  return 0;
}