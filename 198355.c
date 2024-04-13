gnutls_handshake_set_hook_function(gnutls_session_t session,
				   unsigned int htype,
				   int post,
				   gnutls_handshake_hook_func func)
{
	session->internals.h_hook = func;
	session->internals.h_type = htype;
	session->internals.h_post = post;
}