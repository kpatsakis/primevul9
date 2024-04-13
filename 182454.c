assemble_neg_contexts(struct smb2_negotiate_req *req,
		      unsigned int *total_len)
{
	char *pneg_ctxt = (char *)req;
	unsigned int ctxt_len;

	if (*total_len > 200) {
		/* In case length corrupted don't want to overrun smb buffer */
		cifs_dbg(VFS, "Bad frame length assembling neg contexts\n");
		return;
	}

	/*
	 * round up total_len of fixed part of SMB3 negotiate request to 8
	 * byte boundary before adding negotiate contexts
	 */
	*total_len = roundup(*total_len, 8);

	pneg_ctxt = (*total_len) + (char *)req;
	req->NegotiateContextOffset = cpu_to_le32(*total_len);

	build_preauth_ctxt((struct smb2_preauth_neg_context *)pneg_ctxt);
	ctxt_len = DIV_ROUND_UP(sizeof(struct smb2_preauth_neg_context), 8) * 8;
	*total_len += ctxt_len;
	pneg_ctxt += ctxt_len;

	build_encrypt_ctxt((struct smb2_encryption_neg_context *)pneg_ctxt);
	ctxt_len = DIV_ROUND_UP(sizeof(struct smb2_encryption_neg_context), 8) * 8;
	*total_len += ctxt_len;
	pneg_ctxt += ctxt_len;

	build_posix_ctxt((struct smb2_posix_neg_context *)pneg_ctxt);
	*total_len += sizeof(struct smb2_posix_neg_context);

	req->NegotiateContextCount = cpu_to_le16(3);
}