static bool keepalive_fn(const struct timeval *now, void *private_data)
{
	if (!send_keepalive(smbd_server_fd())) {
		DEBUG( 2, ( "Keepalive failed - exiting.\n" ) );
		return False;
	}
	return True;
}