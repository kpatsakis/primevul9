char* x509_proxy_identity_name( globus_gsi_cred_handle_t handle )
{
	char *subject_name = NULL;

	if ( activate_globus_gsi() != 0 ) {
		return NULL;
	}

	if ((*globus_gsi_cred_get_identity_name_ptr)(handle, &subject_name)) {
		set_error_string( "unable to extract identity name" );
		return NULL;
	}

	return subject_name;
}