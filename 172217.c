globus_gsi_cred_handle_t x509_proxy_read( const char *proxy_file )
{
	globus_gsi_cred_handle_t         handle       = NULL;
	globus_gsi_cred_handle_attrs_t   handle_attrs = NULL;
	char *my_proxy_file = NULL;
	bool error = false;

	if ( activate_globus_gsi() != 0 ) {
		return NULL;
	}

	if ((*globus_gsi_cred_handle_attrs_init_ptr)(&handle_attrs)) {
		set_error_string( "problem during internal initialization1" );
		error = true;
		goto cleanup;
	}

	if ((*globus_gsi_cred_handle_init_ptr)(&handle, handle_attrs)) {
		set_error_string( "problem during internal initialization2" );
		error = true;
		goto cleanup;
	}

	/* Check for proxy file */
	if (proxy_file == NULL) {
		my_proxy_file = get_x509_proxy_filename();
		if (my_proxy_file == NULL) {
			goto cleanup;
		}
		proxy_file = my_proxy_file;
	}

	// We should have a proxy file, now, try to read it
	if ((*globus_gsi_cred_read_proxy_ptr)(handle, proxy_file)) {
		set_error_string( "unable to read proxy file" );
		error = true;
		goto cleanup;
	}

 cleanup:
	if (my_proxy_file) {
		free(my_proxy_file);
	}

	if (handle_attrs) {
		(*globus_gsi_cred_handle_attrs_destroy_ptr)(handle_attrs);
	}

	if (error && handle) {
		(*globus_gsi_cred_handle_destroy_ptr)(handle);
		handle = NULL;
	}

	return handle;
}