_gnutls_set_adv_version (gnutls_session_t session, gnutls_protocol_t ver)
{
  set_adv_version (session, _gnutls_version_get_major (ver),
		   _gnutls_version_get_minor (ver));
}