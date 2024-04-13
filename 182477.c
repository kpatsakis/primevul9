static int decode_encrypt_ctx(struct TCP_Server_Info *server,
			      struct smb2_encryption_neg_context *ctxt)
{
	unsigned int len = le16_to_cpu(ctxt->DataLength);

	cifs_dbg(FYI, "decode SMB3.11 encryption neg context of len %d\n", len);
	if (len < MIN_ENCRYPT_CTXT_DATA_LEN) {
		printk_once(KERN_WARNING "server sent bad crypto ctxt len\n");
		return -EINVAL;
	}

	if (le16_to_cpu(ctxt->CipherCount) != 1) {
		printk_once(KERN_WARNING "illegal SMB3.11 cipher count\n");
		return -EINVAL;
	}
	cifs_dbg(FYI, "SMB311 cipher type:%d\n", le16_to_cpu(ctxt->Ciphers[0]));
	if ((ctxt->Ciphers[0] != SMB2_ENCRYPTION_AES128_CCM) &&
	    (ctxt->Ciphers[0] != SMB2_ENCRYPTION_AES128_GCM)) {
		printk_once(KERN_WARNING "invalid SMB3.11 cipher returned\n");
		return -EINVAL;
	}
	server->cipher_type = ctxt->Ciphers[0];
	server->capabilities |= SMB2_GLOBAL_CAP_ENCRYPTION;
	return 0;
}