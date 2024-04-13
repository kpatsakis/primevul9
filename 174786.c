soup_auth_ntlm_class_init (SoupAuthNTLMClass *auth_ntlm_class)
{
	SoupAuthClass *auth_class = SOUP_AUTH_CLASS (auth_ntlm_class);
	SoupConnectionAuthClass *connauth_class = SOUP_CONNECTION_AUTH_CLASS (auth_ntlm_class);
	GObjectClass *object_class = G_OBJECT_CLASS (auth_ntlm_class);

	auth_class->scheme_name = "NTLM";
	auth_class->strength = 3;

	auth_class->get_protection_space = soup_auth_ntlm_get_protection_space;
	auth_class->authenticate = soup_auth_ntlm_authenticate;
	auth_class->is_authenticated = soup_auth_ntlm_is_authenticated;

	connauth_class->create_connection_state = soup_auth_ntlm_create_connection_state;
	connauth_class->free_connection_state = soup_auth_ntlm_free_connection_state;
	connauth_class->update_connection = soup_auth_ntlm_update_connection;
	connauth_class->get_connection_authorization = soup_auth_ntlm_get_connection_authorization;
	connauth_class->is_connection_ready = soup_auth_ntlm_is_connection_ready;

	object_class->finalize = soup_auth_ntlm_finalize;

#ifdef USE_NTLM_AUTH
	ntlm_auth_available = g_file_test (NTLM_AUTH, G_FILE_TEST_IS_EXECUTABLE);
	ntlm_auth_debug = (g_getenv ("SOUP_NTLM_AUTH_DEBUG") != NULL);
#endif
}