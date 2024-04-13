int x509_proxy_seconds_until_expire( globus_gsi_cred_handle_t handle )
{
	time_t time_now;
	time_t time_expire;
	time_t time_diff;

	time_now = time(NULL);
	time_expire = x509_proxy_expiration_time( handle );

	if ( time_expire == -1 ) {
		return -1;
	}

	time_diff = time_expire - time_now;

	if ( time_diff < 0 ) {
		time_diff = 0;
	}

	return (int)time_diff;
}