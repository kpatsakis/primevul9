static void process_smb(struct smbd_server_connection *conn,
			uint8_t *inbuf, size_t nread, size_t unread_bytes,
			uint32_t seqnum, bool encrypted,
			struct smb_perfcount_data *deferred_pcd)
{
	int msg_type = CVAL(inbuf,0);

	DO_PROFILE_INC(smb_count);

	DEBUG( 6, ( "got message type 0x%x of len 0x%x\n", msg_type,
		    smb_len(inbuf) ) );
	DEBUG( 3, ( "Transaction %d of length %d (%u toread)\n", trans_num,
				(int)nread,
				(unsigned int)unread_bytes ));

	if (msg_type != 0) {
		/*
		 * NetBIOS session request, keepalive, etc.
		 */
		reply_special((char *)inbuf);
		goto done;
	}

	if (smbd_server_conn->allow_smb2) {
		if (smbd_is_smb2_header(inbuf, nread)) {
			smbd_smb2_first_negprot(smbd_server_conn, inbuf, nread);
			return;
		}
		smbd_server_conn->allow_smb2 = false;
	}

	show_msg((char *)inbuf);

	construct_reply((char *)inbuf,nread,unread_bytes,seqnum,encrypted,deferred_pcd);
	trans_num++;

done:
	conn->smb1.num_requests++;

	/* The timeout_processing function isn't run nearly
	   often enough to implement 'max log size' without
	   overrunning the size of the file by many megabytes.
	   This is especially true if we are running at debug
	   level 10.  Checking every 50 SMBs is a nice
	   tradeoff of performance vs log file size overrun. */

	if ((conn->smb1.num_requests % 50) == 0 &&
	    need_to_check_log_size()) {
		change_to_root_user();
		check_log_size();
	}
}