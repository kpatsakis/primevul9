smb2_hdr_assemble(struct smb2_sync_hdr *shdr, __le16 smb2_cmd,
		  const struct cifs_tcon *tcon)
{
	shdr->ProtocolId = SMB2_PROTO_NUMBER;
	shdr->StructureSize = cpu_to_le16(64);
	shdr->Command = smb2_cmd;
	if (tcon && tcon->ses && tcon->ses->server) {
		struct TCP_Server_Info *server = tcon->ses->server;

		spin_lock(&server->req_lock);
		/* Request up to 10 credits but don't go over the limit. */
		if (server->credits >= server->max_credits)
			shdr->CreditRequest = cpu_to_le16(0);
		else
			shdr->CreditRequest = cpu_to_le16(
				min_t(int, server->max_credits -
						server->credits, 10));
		spin_unlock(&server->req_lock);
	} else {
		shdr->CreditRequest = cpu_to_le16(2);
	}
	shdr->ProcessId = cpu_to_le32((__u16)current->tgid);

	if (!tcon)
		goto out;

	/* GLOBAL_CAP_LARGE_MTU will only be set if dialect > SMB2.02 */
	/* See sections 2.2.4 and 3.2.4.1.5 of MS-SMB2 */
	if ((tcon->ses) && (tcon->ses->server) &&
	    (tcon->ses->server->capabilities & SMB2_GLOBAL_CAP_LARGE_MTU))
		shdr->CreditCharge = cpu_to_le16(1);
	/* else CreditCharge MBZ */

	shdr->TreeId = tcon->tid;
	/* Uid is not converted */
	if (tcon->ses)
		shdr->SessionId = tcon->ses->Suid;

	/*
	 * If we would set SMB2_FLAGS_DFS_OPERATIONS on open we also would have
	 * to pass the path on the Open SMB prefixed by \\server\share.
	 * Not sure when we would need to do the augmented path (if ever) and
	 * setting this flag breaks the SMB2 open operation since it is
	 * illegal to send an empty path name (without \\server\share prefix)
	 * when the DFS flag is set in the SMB open header. We could
	 * consider setting the flag on all operations other than open
	 * but it is safer to net set it for now.
	 */
/*	if (tcon->share_flags & SHI1005_FLAGS_DFS)
		shdr->Flags |= SMB2_FLAGS_DFS_OPERATIONS; */

	if (tcon->ses && tcon->ses->server && tcon->ses->server->sign &&
	    !smb3_encryption_required(tcon))
		shdr->Flags |= SMB2_FLAGS_SIGNED;
out:
	return;
}