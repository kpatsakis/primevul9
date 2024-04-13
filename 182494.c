static int smb311_decode_neg_context(struct smb2_negotiate_rsp *rsp,
				     struct TCP_Server_Info *server,
				     unsigned int len_of_smb)
{
	struct smb2_neg_context *pctx;
	unsigned int offset = le32_to_cpu(rsp->NegotiateContextOffset);
	unsigned int ctxt_cnt = le16_to_cpu(rsp->NegotiateContextCount);
	unsigned int len_of_ctxts, i;
	int rc = 0;

	cifs_dbg(FYI, "decoding %d negotiate contexts\n", ctxt_cnt);
	if (len_of_smb <= offset) {
		cifs_dbg(VFS, "Invalid response: negotiate context offset\n");
		return -EINVAL;
	}

	len_of_ctxts = len_of_smb - offset;

	for (i = 0; i < ctxt_cnt; i++) {
		int clen;
		/* check that offset is not beyond end of SMB */
		if (len_of_ctxts == 0)
			break;

		if (len_of_ctxts < sizeof(struct smb2_neg_context))
			break;

		pctx = (struct smb2_neg_context *)(offset + (char *)rsp);
		clen = le16_to_cpu(pctx->DataLength);
		if (clen > len_of_ctxts)
			break;

		if (pctx->ContextType == SMB2_PREAUTH_INTEGRITY_CAPABILITIES)
			decode_preauth_context(
				(struct smb2_preauth_neg_context *)pctx);
		else if (pctx->ContextType == SMB2_ENCRYPTION_CAPABILITIES)
			rc = decode_encrypt_ctx(server,
				(struct smb2_encryption_neg_context *)pctx);
		else if (pctx->ContextType == SMB2_POSIX_EXTENSIONS_AVAILABLE)
			server->posix_ext_supported = true;
		else
			cifs_dbg(VFS, "unknown negcontext of type %d ignored\n",
				le16_to_cpu(pctx->ContextType));

		if (rc)
			break;
		/* offsets must be 8 byte aligned */
		clen = (clen + 7) & ~0x7;
		offset += clen + sizeof(struct smb2_neg_context);
		len_of_ctxts -= clen;
	}
	return rc;
}