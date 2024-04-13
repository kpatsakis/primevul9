x509_send_delegation( const char *source_file,
					  time_t expiration_time,
					  time_t *result_expiration_time,
					  int (*recv_data_func)(void *, void **, size_t *), 
					  void *recv_data_ptr,
					  int (*send_data_func)(void *, void *, size_t),
					  void *send_data_ptr )
{
#if !defined(HAVE_EXT_GLOBUS)
	(void) source_file;
	(void) expiration_time;
	(void) result_expiration_time;
	(void) recv_data_func;
	(void) recv_data_ptr;
	(void) send_data_func;
	(void) send_data_ptr;

	_globus_error_message =
		strdup( NOT_SUPPORTED_MSG );
	return -1;

#else
	int rc = 0;
	int error_line = 0;
	globus_result_t result = GLOBUS_SUCCESS;
	globus_gsi_cred_handle_t source_cred =  NULL;
	globus_gsi_proxy_handle_t new_proxy = NULL;
	char *buffer = NULL;
	size_t buffer_len = 0;
	BIO *bio = NULL;
	X509 *cert = NULL;
	STACK_OF(X509) *cert_chain = NULL;
	int idx = 0;
	globus_gsi_cert_utils_cert_type_t cert_type;
	int is_limited;

	if ( activate_globus_gsi() != 0 ) {
		return -1;
	}

	result = (*globus_gsi_cred_handle_init_ptr)( &source_cred, NULL );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	result = (*globus_gsi_proxy_handle_init_ptr)( &new_proxy, NULL );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	result = (*globus_gsi_cred_read_proxy_ptr)( source_cred, source_file );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

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

	free( buffer );
	buffer = NULL;

	result = (*globus_gsi_proxy_inquire_req_ptr)( new_proxy, bio );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	BIO_free( bio );
	bio = NULL;

		// modify certificate properties
		// set the appropriate proxy type
	result = (*globus_gsi_cred_get_cert_type_ptr)( source_cred, &cert_type );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}
	switch ( cert_type ) {
	case GLOBUS_GSI_CERT_UTILS_TYPE_CA:
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	case GLOBUS_GSI_CERT_UTILS_TYPE_EEC:
	case GLOBUS_GSI_CERT_UTILS_TYPE_GSI_3_INDEPENDENT_PROXY:
	case GLOBUS_GSI_CERT_UTILS_TYPE_GSI_3_RESTRICTED_PROXY:
		cert_type = GLOBUS_GSI_CERT_UTILS_TYPE_GSI_3_IMPERSONATION_PROXY;
		break;
	case GLOBUS_GSI_CERT_UTILS_TYPE_RFC_INDEPENDENT_PROXY:
	case GLOBUS_GSI_CERT_UTILS_TYPE_RFC_RESTRICTED_PROXY:
		cert_type = GLOBUS_GSI_CERT_UTILS_TYPE_RFC_IMPERSONATION_PROXY;
		break;
	case GLOBUS_GSI_CERT_UTILS_TYPE_GSI_3_IMPERSONATION_PROXY:
	case GLOBUS_GSI_CERT_UTILS_TYPE_GSI_3_LIMITED_PROXY:
	case GLOBUS_GSI_CERT_UTILS_TYPE_GSI_2_PROXY:
	case GLOBUS_GSI_CERT_UTILS_TYPE_GSI_2_LIMITED_PROXY:
	case GLOBUS_GSI_CERT_UTILS_TYPE_RFC_IMPERSONATION_PROXY:
	case GLOBUS_GSI_CERT_UTILS_TYPE_RFC_LIMITED_PROXY:
	default:
			// Use the same certificate type
		break;
	}
	result = (*globus_gsi_proxy_handle_set_type_ptr)( new_proxy, cert_type);
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	// see if this should be made a limited proxy
	is_limited = !(param_boolean_int("DELEGATE_FULL_JOB_GSI_CREDENTIALS", 0));
	if (is_limited) {
		result = (*globus_gsi_proxy_handle_set_is_limited_ptr)( new_proxy, GLOBUS_TRUE);
		if ( result != GLOBUS_SUCCESS ) {
			rc = -1;
			error_line = __LINE__;
			goto cleanup;
		}
	}

	if( expiration_time || result_expiration_time ) {
		time_t time_left = 0;
		result = (*globus_gsi_cred_get_lifetime_ptr)( source_cred, &time_left );
		if ( result != GLOBUS_SUCCESS ) {
			rc = -1;
			error_line = __LINE__;
			goto cleanup;
		}

		time_t now = time(NULL);
		int orig_expiration_time = now + time_left;

		if( result_expiration_time ) {
			*result_expiration_time = orig_expiration_time;
		}

		if( expiration_time && orig_expiration_time > expiration_time ) {
			int time_valid = (expiration_time - now)/60;

			result = (*globus_gsi_proxy_handle_set_time_valid_ptr)( new_proxy, time_valid );
			if ( result != GLOBUS_SUCCESS ) {
				rc = -1;
				error_line = __LINE__;
				goto cleanup;
			}
			if( result_expiration_time ) {
				*result_expiration_time = expiration_time;
			}
		}
	}

	/* TODO Do we have to destroy and re-create bio, or can we reuse it? */
	bio = BIO_new( BIO_s_mem() );
	if ( bio == NULL ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	result = (*globus_gsi_proxy_sign_req_ptr)( new_proxy, source_cred, bio );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

		// Now we need to stuff the certificate chain into in the bio.
		// This consists of the signed certificate and its whole chain.
	result = (*globus_gsi_cred_get_cert_ptr)( source_cred, &cert );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}
	i2d_X509_bio( bio, cert );
	X509_free( cert );
	cert = NULL;

	result = (*globus_gsi_cred_get_cert_chain_ptr)( source_cred, &cert_chain );
	if ( result != GLOBUS_SUCCESS ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	for( idx = 0; idx < sk_X509_num( cert_chain ); idx++ ) {
		X509 *next_cert;
		next_cert = sk_X509_value( cert_chain, idx );
		i2d_X509_bio( bio, next_cert );
	}
	sk_X509_pop_free( cert_chain, X509_free );
	cert_chain = NULL;

	if ( bio_to_buffer( bio, &buffer, &buffer_len ) == FALSE ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

	if ( send_data_func( send_data_ptr, buffer, buffer_len ) != 0 ) {
		rc = -1;
		error_line = __LINE__;
		goto cleanup;
	}

 cleanup:
	/* TODO Extract Globus error message if result isn't GLOBUS_SUCCESS */
	if ( error_line ) {
		char buff[1024];
		snprintf( buff, sizeof(buff), "x509_send_delegation failed at line %d",
				  error_line );
		set_error_string( buff );
	}

	if ( bio ) {
		BIO_free( bio );
	}
	if ( buffer ) {
		free( buffer );
	}
	if ( new_proxy ) {
		(*globus_gsi_proxy_handle_destroy_ptr)( new_proxy );
	}
	if ( source_cred ) {
		(*globus_gsi_cred_handle_destroy_ptr)( source_cred );
	}
	if ( cert ) {
		X509_free( cert );
	}
	if ( cert_chain ) {
		sk_X509_pop_free( cert_chain, X509_free );
	}

	return rc;
#endif
}