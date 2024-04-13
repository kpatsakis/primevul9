static connection_struct *switch_message(uint8 type, struct smb_request *req, int size)
{
	int flags;
	uint16 session_tag;
	connection_struct *conn = NULL;
	struct smbd_server_connection *sconn = smbd_server_conn;

	errno = 0;

	/* Make sure this is an SMB packet. smb_size contains NetBIOS header
	 * so subtract 4 from it. */
	if (!valid_smb_header(req->inbuf)
	    || (size < (smb_size - 4))) {
		DEBUG(2,("Non-SMB packet of length %d. Terminating server\n",
			 smb_len(req->inbuf)));
		exit_server_cleanly("Non-SMB packet");
	}

	if (smb_messages[type].fn == NULL) {
		DEBUG(0,("Unknown message type %d!\n",type));
		smb_dump("Unknown", 1, (char *)req->inbuf, size);
		reply_unknown_new(req, type);
		return NULL;
	}

	flags = smb_messages[type].flags;

	/* In share mode security we must ignore the vuid. */
	session_tag = (lp_security() == SEC_SHARE)
		? UID_FIELD_INVALID : req->vuid;
	conn = req->conn;

	DEBUG(3,("switch message %s (pid %d) conn 0x%lx\n", smb_fn_name(type),
		 (int)sys_getpid(), (unsigned long)conn));

	smb_dump(smb_fn_name(type), 1, (char *)req->inbuf, size);

	/* Ensure this value is replaced in the incoming packet. */
	SSVAL(req->inbuf,smb_uid,session_tag);

	/*
	 * Ensure the correct username is in current_user_info.  This is a
	 * really ugly bugfix for problems with multiple session_setup_and_X's
	 * being done and allowing %U and %G substitutions to work correctly.
	 * There is a reason this code is done here, don't move it unless you
	 * know what you're doing... :-).
	 * JRA.
	 */

	if (session_tag != sconn->smb1.sessions.last_session_tag) {
		user_struct *vuser = NULL;

		sconn->smb1.sessions.last_session_tag = session_tag;
		if(session_tag != UID_FIELD_INVALID) {
			vuser = get_valid_user_struct(sconn, session_tag);
			if (vuser) {
				set_current_user_info(
					vuser->server_info->sanitized_username,
					vuser->server_info->unix_name,
					pdb_get_domain(vuser->server_info
						       ->sam_account));
			}
		}
	}

	/* Does this call need to be run as the connected user? */
	if (flags & AS_USER) {

		/* Does this call need a valid tree connection? */
		if (!conn) {
			/*
			 * Amazingly, the error code depends on the command
			 * (from Samba4).
			 */
			if (type == SMBntcreateX) {
				reply_nterror(req, NT_STATUS_INVALID_HANDLE);
			} else {
				reply_doserror(req, ERRSRV, ERRinvnid);
			}
			return NULL;
		}

		if (!change_to_user(conn,session_tag)) {
			reply_nterror(req, NT_STATUS_DOS(ERRSRV, ERRbaduid));
			remove_deferred_open_smb_message(req->mid);
			return conn;
		}

		/* All NEED_WRITE and CAN_IPC flags must also have AS_USER. */

		/* Does it need write permission? */
		if ((flags & NEED_WRITE) && !CAN_WRITE(conn)) {
			reply_nterror(req, NT_STATUS_MEDIA_WRITE_PROTECTED);
			return conn;
		}

		/* IPC services are limited */
		if (IS_IPC(conn) && !(flags & CAN_IPC)) {
			reply_doserror(req, ERRSRV,ERRaccess);
			return conn;
		}
	} else {
		/* This call needs to be run as root */
		change_to_root_user();
	}

	/* load service specific parameters */
	if (conn) {
		if (req->encrypted) {
			conn->encrypted_tid = true;
			/* encrypted required from now on. */
			conn->encrypt_level = Required;
		} else if (ENCRYPTION_REQUIRED(conn)) {
			if (req->cmd != SMBtrans2 && req->cmd != SMBtranss2) {
				exit_server_cleanly("encryption required "
					"on connection");
				return conn;
			}
		}

		if (!set_current_service(conn,SVAL(req->inbuf,smb_flg),
					 (flags & (AS_USER|DO_CHDIR)
					  ?True:False))) {
			reply_doserror(req, ERRSRV, ERRaccess);
			return conn;
		}
		conn->num_smb_operations++;
	}

	/* does this protocol need to be run as guest? */
	if ((flags & AS_GUEST)
	    && (!change_to_guest() ||
		!check_access(smbd_server_fd(), lp_hostsallow(-1),
			      lp_hostsdeny(-1)))) {
		reply_doserror(req, ERRSRV, ERRaccess);
		return conn;
	}

	smb_messages[type].fn(req);
	return req->conn;
}