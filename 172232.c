x509_proxy_expiration_time( const char *proxy_file )
{
#if !defined(HAVE_EXT_GLOBUS)
	(void) proxy_file;
	set_error_string( NOT_SUPPORTED_MSG );
	return -1;
#else

	time_t expiration_time = -1;
	globus_gsi_cred_handle_t proxy_handle = x509_proxy_read( proxy_file );

	if ( proxy_handle == NULL ) {
		return -1;
	}

	expiration_time = x509_proxy_expiration_time( proxy_handle );

	x509_proxy_free( proxy_handle );

	return expiration_time;

#endif /* !defined(GSS_AUTHENTICATION) */
}