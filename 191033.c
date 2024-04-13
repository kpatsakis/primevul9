static bool deadtime_fn(const struct timeval *now, void *private_data)
{
	struct smbd_server_connection *sconn = smbd_server_conn;
	if ((conn_num_open(sconn) == 0)
	    || (conn_idle_all(sconn, now->tv_sec))) {
		DEBUG( 2, ( "Closing idle connection\n" ) );
		messaging_send(smbd_messaging_context(), procid_self(),
			       MSG_SHUTDOWN, &data_blob_null);
		return False;
	}

	return True;
}