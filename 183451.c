_gnutls_get_adv_version (gnutls_session_t session)
{
  return _gnutls_version_get (_gnutls_get_adv_version_major (session),
			      _gnutls_get_adv_version_minor (session));
}