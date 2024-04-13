build_preauth_ctxt(struct smb2_preauth_neg_context *pneg_ctxt)
{
	pneg_ctxt->ContextType = SMB2_PREAUTH_INTEGRITY_CAPABILITIES;
	pneg_ctxt->DataLength = cpu_to_le16(38);
	pneg_ctxt->HashAlgorithmCount = cpu_to_le16(1);
	pneg_ctxt->SaltLength = cpu_to_le16(SMB311_SALT_SIZE);
	get_random_bytes(pneg_ctxt->Salt, SMB311_SALT_SIZE);
	pneg_ctxt->HashAlgorithms = SMB2_PREAUTH_INTEGRITY_SHA512;
}