x509_proxy_email( const char *proxy_file )
{
#if !defined(HAVE_EXT_GLOBUS)
	(void) proxy_file;
	set_error_string( NOT_SUPPORTED_MSG );
	return NULL;
#else

	char *email = NULL;
	globus_gsi_cred_handle_t proxy_handle = x509_proxy_read( proxy_file );

	if ( proxy_handle == NULL ) {
		return NULL;
	}

	email = x509_proxy_email( proxy_handle );

	x509_proxy_free( proxy_handle );

	return email;
#endif /* !defined(HAVE_EXT_GLOBUS) */
}