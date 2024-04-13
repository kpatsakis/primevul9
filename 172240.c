extract_VOMS_info_from_file( const char* proxy_file, int verify_type, char **voname, char **firstfqan, char **quoted_DN_and_FQAN)
{

	globus_gsi_cred_handle_t         handle       = NULL;
	globus_gsi_cred_handle_attrs_t   handle_attrs = NULL;
	char *my_proxy_file = NULL;
	int error = 0;

	if ( activate_globus_gsi() != 0 ) {
		return 2;
	}

	if ((*globus_gsi_cred_handle_attrs_init_ptr)(&handle_attrs)) {
		set_error_string( "problem during internal initialization1" );
		error = 3;
		goto cleanup;
	}

	if ((*globus_gsi_cred_handle_init_ptr)(&handle, handle_attrs)) {
		set_error_string( "problem during internal initialization2" );
		error = 4;
		goto cleanup;
	}

	/* Check for proxy file */
	if (proxy_file == NULL) {
		my_proxy_file = get_x509_proxy_filename();
		if (my_proxy_file == NULL) {
			error = 5;
			goto cleanup;
		}
		proxy_file = my_proxy_file;
	}

	// We should have a proxy file, now, try to read it
	if ((*globus_gsi_cred_read_proxy_ptr)(handle, proxy_file)) {
		set_error_string( "unable to read proxy file" );
		error = 6;
		goto cleanup;
	}

	error = extract_VOMS_info( handle, verify_type, voname, firstfqan, quoted_DN_and_FQAN );


 cleanup:
	if (my_proxy_file) {
		free(my_proxy_file);
	}

	if (handle_attrs) {
		(*globus_gsi_cred_handle_attrs_destroy_ptr)(handle_attrs);
	}

	if (handle) {
		(*globus_gsi_cred_handle_destroy_ptr)(handle);
	}

	return error; // success

}