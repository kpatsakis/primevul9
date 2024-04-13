time_t x509_proxy_expiration_time( globus_gsi_cred_handle_t handle )
{
	time_t time_left;

	if ( activate_globus_gsi() != 0 ) {
		return -1;
	}

	if ((*globus_gsi_cred_get_lifetime_ptr)(handle, &time_left)) {
		set_error_string( "unable to extract expiration time" );
		return -1;
    }

	return time(NULL) + time_left;
}