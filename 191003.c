void smbd_process(void)
{
	TALLOC_CTX *frame = talloc_stackframe();
	char remaddr[INET6_ADDRSTRLEN];

	if (lp_maxprotocol() == PROTOCOL_SMB2 &&
	    lp_security() != SEC_SHARE) {
		smbd_server_conn->allow_smb2 = true;
	}

	/* Ensure child is set to blocking mode */
	set_blocking(smbd_server_fd(),True);

	set_socket_options(smbd_server_fd(),"SO_KEEPALIVE");
	set_socket_options(smbd_server_fd(), lp_socket_options());

	/* this is needed so that we get decent entries
	   in smbstatus for port 445 connects */
	set_remote_machine_name(get_peer_addr(smbd_server_fd(),
					      remaddr,
					      sizeof(remaddr)),
					      false);
	reload_services(true);

	/*
	 * Before the first packet, check the global hosts allow/ hosts deny
	 * parameters before doing any parsing of packets passed to us by the
	 * client. This prevents attacks on our parsing code from hosts not in
	 * the hosts allow list.
	 */

	if (!check_access(smbd_server_fd(), lp_hostsallow(-1),
			  lp_hostsdeny(-1))) {
		char addr[INET6_ADDRSTRLEN];

		/*
		 * send a negative session response "not listening on calling
		 * name"
		 */
		unsigned char buf[5] = {0x83, 0, 0, 1, 0x81};
		DEBUG( 1, ("Connection denied from %s\n",
			   client_addr(get_client_fd(),addr,sizeof(addr)) ) );
		(void)srv_send_smb(smbd_server_fd(),(char *)buf, false,
				   0, false, NULL);
		exit_server_cleanly("connection denied");
	}

	static_init_rpc;

	init_modules();

	smb_perfcount_init();

	if (!init_account_policy()) {
		exit_server("Could not open account policy tdb.\n");
	}

	if (*lp_rootdir()) {
		if (chroot(lp_rootdir()) != 0) {
			DEBUG(0,("Failed to change root to %s\n", lp_rootdir()));
			exit_server("Failed to chroot()");
		}
		if (chdir("/") == -1) {
			DEBUG(0,("Failed to chdir to / on chroot to %s\n", lp_rootdir()));
			exit_server("Failed to chroot()");
		}
		DEBUG(0,("Changed root to %s\n", lp_rootdir()));
	}

	if (!srv_init_signing(smbd_server_conn)) {
		exit_server("Failed to init smb_signing");
	}

	/* Setup oplocks */
	if (!init_oplocks(smbd_messaging_context()))
		exit_server("Failed to init oplocks");

	/* Setup aio signal handler. */
	initialize_async_io_handler();

	/* register our message handlers */
	messaging_register(smbd_messaging_context(), NULL,
			   MSG_SMB_FORCE_TDIS, msg_force_tdis);
	messaging_register(smbd_messaging_context(), NULL,
			   MSG_SMB_RELEASE_IP, msg_release_ip);
	messaging_register(smbd_messaging_context(), NULL,
			   MSG_SMB_CLOSE_FILE, msg_close_file);

	/*
	 * Use the default MSG_DEBUG handler to avoid rebroadcasting
	 * MSGs to all child processes
	 */
	messaging_deregister(smbd_messaging_context(),
			     MSG_DEBUG, NULL);
	messaging_register(smbd_messaging_context(), NULL,
			   MSG_DEBUG, debug_message);

	if ((lp_keepalive() != 0)
	    && !(event_add_idle(smbd_event_context(), NULL,
				timeval_set(lp_keepalive(), 0),
				"keepalive", keepalive_fn,
				NULL))) {
		DEBUG(0, ("Could not add keepalive event\n"));
		exit(1);
	}

	if (!(event_add_idle(smbd_event_context(), NULL,
			     timeval_set(IDLE_CLOSED_TIMEOUT, 0),
			     "deadtime", deadtime_fn, NULL))) {
		DEBUG(0, ("Could not add deadtime event\n"));
		exit(1);
	}

	if (!(event_add_idle(smbd_event_context(), NULL,
			     timeval_set(SMBD_SELECT_TIMEOUT, 0),
			     "housekeeping", housekeeping_fn, NULL))) {
		DEBUG(0, ("Could not add housekeeping event\n"));
		exit(1);
	}

#ifdef CLUSTER_SUPPORT

	if (lp_clustering()) {
		/*
		 * We need to tell ctdb about our client's TCP
		 * connection, so that for failover ctdbd can send
		 * tickle acks, triggering a reconnection by the
		 * client.
		 */

		struct sockaddr_storage srv, clnt;

		if (client_get_tcp_info(&srv, &clnt) == 0) {

			NTSTATUS status;

			status = ctdbd_register_ips(
				messaging_ctdbd_connection(),
				&srv, &clnt, release_ip, NULL);

			if (!NT_STATUS_IS_OK(status)) {
				DEBUG(0, ("ctdbd_register_ips failed: %s\n",
					  nt_errstr(status)));
			}
		} else
		{
			DEBUG(0,("Unable to get tcp info for "
				 "CTDB_CONTROL_TCP_CLIENT: %s\n",
				 strerror(errno)));
		}
	}

#endif

	smbd_server_conn->nbt.got_session = false;

	smbd_server_conn->smb1.negprot.max_recv = MIN(lp_maxxmit(),BUFFER_SIZE);

	smbd_server_conn->smb1.sessions.done_sesssetup = false;
	smbd_server_conn->smb1.sessions.max_send = BUFFER_SIZE;
	smbd_server_conn->smb1.sessions.last_session_tag = UID_FIELD_INVALID;
	/* users from session setup */
	smbd_server_conn->smb1.sessions.session_userlist = NULL;
	/* workgroup from session setup. */
	smbd_server_conn->smb1.sessions.session_workgroup = NULL;
	/* this holds info on user ids that are already validated for this VC */
	smbd_server_conn->smb1.sessions.validated_users = NULL;
	smbd_server_conn->smb1.sessions.next_vuid = VUID_OFFSET;
	smbd_server_conn->smb1.sessions.num_validated_vuids = 0;
#ifdef HAVE_NETGROUP
	smbd_server_conn->smb1.sessions.my_yp_domain = NULL;
#endif

	conn_init(smbd_server_conn);

	smbd_server_conn->smb1.fde = event_add_fd(smbd_event_context(),
						  smbd_server_conn,
						  smbd_server_fd(),
						  EVENT_FD_READ,
						  smbd_server_connection_handler,
						  smbd_server_conn);
	if (!smbd_server_conn->smb1.fde) {
		exit_server("failed to create smbd_server_connection fde");
	}

	TALLOC_FREE(frame);

	while (True) {
		NTSTATUS status;

		frame = talloc_stackframe_pool(8192);

		errno = 0;

		status = smbd_server_connection_loop_once(smbd_server_conn);
		if (!NT_STATUS_EQUAL(status, NT_STATUS_RETRY) &&
		    !NT_STATUS_IS_OK(status)) {
			DEBUG(3, ("smbd_server_connection_loop_once failed: %s,"
				  " exiting\n", nt_errstr(status)));
			break;
		}

		TALLOC_FREE(frame);
	}

	exit_server_cleanly(NULL);
}