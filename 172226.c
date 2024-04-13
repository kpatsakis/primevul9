void x509_proxy_free( globus_gsi_cred_handle_t handle )
{
	if ( activate_globus_gsi() != 0 ) {
		return;
	}
	if (handle) {
		(*globus_gsi_cred_handle_destroy_ptr)(handle);
	}
}