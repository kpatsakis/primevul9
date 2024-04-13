x509_receive_delegation( const char *destination_file,
						 int (*recv_data_func)(void *, void **, size_t *), 
						 void *recv_data_ptr,
						 int (*send_data_func)(void *, void *, size_t),
						 void *send_data_ptr,
						 void ** state_ptr )
{
#if !defined(HAVE_EXT_GLOBUS)
	(void) destination_file;		// Quiet compiler warnings
	(void) recv_data_func;			// Quiet compiler warnings
	(void) recv_data_ptr;			// Quiet compiler warnings
	(void) send_data_func;			// Quiet compiler warnings
	(void) send_data_ptr;			// Quiet compiler warnings
	(void) state_ptr;			// Quiet compiler warnings
	_globus_error_message =
		strdup( NOT_SUPPORTED_MSG );
	return -1;

#else
	int rc = 0;
	int error_line = 0;
	x509_delegation_state *st = new x509_delegation_state();
	st->m_dest = strdup(destination_file);
	globus_result_t result = GLOBUS_SUCCESS;
	st->m_request_handle = NULL;
	globus_gsi_proxy_handle_attrs_t handle_attrs = NULL;
	char *buffer = NULL;
	size_t buffer_len = 0;
	BIO *bio = NULL;

	if ( activate_globus_gsi() != 0 ) {
		if ( st->m_dest ) { free(st->m_dest); }
		delete st;
		return -1;
	}

	// declare some vars we'll need
	int globus_bits = 0;
	int bits = 0;
	int skew = 0;

	// prepare any special attributes desired
	result = (*globus_gsi_proxy_handle_attrs_init_ptr)( &handle_attrs );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	// first, get the default that globus is using
	result = (*globus_gsi_proxy_handle_attrs_get_keybits_ptr)( handle_attrs, &globus_bits );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	// as of 2014-01-16, many various pieces of the OSG software stack no
	// longer work with proxies less than 1024 bits, so make sure globus is
	// defaulting to at least that large
	if (globus_bits < 1024) {
		globus_bits = 1024;
		result = (*globus_gsi_proxy_handle_attrs_set_keybits_ptr)( handle_attrs, globus_bits );
		if ( result != GLOBUS_SUCCESS ) {
			rc = -1;
			error_line = __LINE__;
			goto cleanup;
		}
	}

	// also allow the condor admin to increase it if they really feel the need
	bits = param_integer("GSI_DELEGATION_KEYBITS", 0);
	if (bits > globus_bits) {
		result = (*globus_gsi_proxy_handle_attrs_set_keybits_ptr)( handle_attrs, bits );
		if ( result != GLOBUS_SUCCESS ) {
			rc = -1;
			error_line = __LINE__;
			goto cleanup;
		}
	}

	// default for clock skew is currently (2013-03-27) 5 minutes, but allow
	// that to be changed
	skew = param_integer("GSI_DELEGATION_CLOCK_SKEW_ALLOWABLE", 0);

	if (skew) {
		result = (*globus_gsi_proxy_handle_attrs_set_clock_skew_allowable_ptr)( handle_attrs, skew );
		if ( result != GLOBUS_SUCCESS ) {
			rc = -1;
			error_line = __LINE__;
			goto cleanup;
		}
	}

	// Note: inspecting the Globus implementation, globus_gsi_proxy_handle_init creates a copy
	// of handle_attrs; hence, it's OK for handle_attrs to be destroyed before m_request_handle.
	result = (*globus_gsi_proxy_handle_init_ptr)( &(st->m_request_handle), handle_attrs );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	bio = BIO_new( BIO_s_mem() );
	if ( bio == NULL ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	result = (*globus_gsi_proxy_create_req_ptr)( st->m_request_handle, bio );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}


	if ( bio_to_buffer( bio, &buffer, &buffer_len ) == FALSE ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	BIO_free( bio );
	bio = NULL;

	if ( send_data_func( send_data_ptr, buffer, buffer_len ) != 0 ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	free( buffer );
	buffer = NULL;

cleanup:
	/* TODO Extract Globus error message if result isn't GLOBUS_SUCCESS */
	if ( error_line ) {
		char buff[1024];
		snprintf( buff, sizeof(buff), "x509_receive_delegation failed "
			"at line %d", error_line );
		buff[1023] = '\0';
		set_error_string( buff );
	}

	if ( bio ) {
		BIO_free( bio );
	}
	if ( buffer ) {
		free( buffer );
	}
	if (handle_attrs) {
		(*globus_gsi_proxy_handle_attrs_destroy_ptr)( handle_attrs );
	}
	// Error!  Cleanup memory immediately and return.
	if ( rc && st ) {
		if ( st->m_request_handle ) {
			(*globus_gsi_proxy_handle_destroy_ptr)( st->m_request_handle );
		}
		if ( st->m_dest ) { free(st->m_dest); }
		delete st;
		return rc;
	}

	// We were given a state pointer - caller will take care of monitoring the
	// socket for more data and call delegation_finish later.
	if (state_ptr != NULL) {
		*state_ptr = st;
		return 2;
	}

	// Else, we block and finish up immediately.
	return x509_receive_delegation_finish(recv_data_func, recv_data_ptr, &st);
#endif
}