int x509_receive_delegation_finish(int (*recv_data_func)(void *, void **, size_t *),
                               void *recv_data_ptr,
                               void *state_ptr_raw)
{
#if !defined(HAVE_EXT_GLOBUS)
	(void) recv_data_func;			// Quiet compiler warnings
	(void) recv_data_ptr;			// Quiet compiler warnings
	(void) state_ptr_raw;			// Quiet compiler warnings
	_globus_error_message =
		strdup( NOT_SUPPORTED_MSG );
	return -1;

#else
	x509_delegation_state *state_ptr = static_cast<x509_delegation_state*>(state_ptr_raw);
	globus_result_t result = GLOBUS_SUCCESS;
	globus_gsi_cred_handle_t proxy_handle =  NULL;
	int rc = 0;
	int error_line = 0;
	char *buffer = NULL;
	size_t buffer_len = 0;
	BIO *bio = NULL;

	if ( recv_data_func( recv_data_ptr, (void **)&buffer, &buffer_len ) != 0 ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	if ( buffer_to_bio( buffer, buffer_len, &bio ) == FALSE ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	result = (*globus_gsi_proxy_assemble_cred_ptr)( state_ptr->m_request_handle, &proxy_handle,
	                                                bio );

	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	/* globus_gsi_cred_write_proxy() declares its second argument non-const,
	 * but never modifies it. The copy gets rid of compiler warnings.
	 */
	result = (*globus_gsi_cred_write_proxy_ptr)( proxy_handle, state_ptr->m_dest );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

 cleanup:
	/* TODO Extract Globus error message if result isn't GLOBUS_SUCCESS */
	if ( error_line ) {
		char buff[1024];
		snprintf( buff, sizeof(buff), "x509_receive_delegation failed "
				  "at line %d", error_line );
		set_error_string( buff );
	}

	if ( bio ) {
		BIO_free( bio );
	}
	if ( buffer ) {
		free( buffer );
	}
	if ( state_ptr ) {
		if ( state_ptr->m_request_handle ) {
			(*globus_gsi_proxy_handle_destroy_ptr)( state_ptr->m_request_handle );
		}
		if ( state_ptr->m_dest ) { free(state_ptr->m_dest); }
		delete state_ptr;
	}
	if ( proxy_handle ) {
		(*globus_gsi_cred_handle_destroy_ptr)( proxy_handle );
	}

	return rc;
#endif
}