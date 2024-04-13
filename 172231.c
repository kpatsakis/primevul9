get_x509_proxy_filename( void )
{
	char *proxy_file = NULL;
#if defined(HAVE_EXT_GLOBUS)
	globus_gsi_proxy_file_type_t     file_type    = GLOBUS_PROXY_FILE_INPUT;

	if ( activate_globus_gsi() != 0 ) {
		return NULL;
	}

	if ( (*globus_gsi_sysconfig_get_proxy_filename_unix_ptr)(&proxy_file, file_type) !=
		 GLOBUS_SUCCESS ) {
		set_error_string( "unable to locate proxy file" );
	}
#endif
	return proxy_file;
}