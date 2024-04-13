x509_proxy_identity_name( const char *proxy_file )
{
#if !defined(HAVE_EXT_GLOBUS)
	(void) proxy_file;
	set_error_string( NOT_SUPPORTED_MSG );
	return NULL;
#else

	char *subject_name = NULL;
	globus_gsi_cred_handle_t proxy_handle = x509_proxy_read( proxy_file );

	if ( proxy_handle == NULL ) {
		return NULL;
	}

	subject_name = x509_proxy_identity_name( proxy_handle );

	x509_proxy_free( proxy_handle );

	return subject_name;

#endif /* !defined(GSS_AUTHENTICATION) */
}