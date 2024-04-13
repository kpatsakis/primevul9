build_encrypt_ctxt(struct smb2_encryption_neg_context *pneg_ctxt)
{
	pneg_ctxt->ContextType = SMB2_ENCRYPTION_CAPABILITIES;
	pneg_ctxt->DataLength = cpu_to_le16(4); /* Cipher Count + le16 cipher */
	pneg_ctxt->CipherCount = cpu_to_le16(1);
/* pneg_ctxt->Ciphers[0] = SMB2_ENCRYPTION_AES128_GCM;*/ /* not supported yet */
	pneg_ctxt->Ciphers[0] = SMB2_ENCRYPTION_AES128_CCM;
}