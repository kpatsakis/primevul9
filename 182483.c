static void decode_preauth_context(struct smb2_preauth_neg_context *ctxt)
{
	unsigned int len = le16_to_cpu(ctxt->DataLength);

	/* If invalid preauth context warn but use what we requested, SHA-512 */
	if (len < MIN_PREAUTH_CTXT_DATA_LEN) {
		printk_once(KERN_WARNING "server sent bad preauth context\n");
		return;
	}
	if (le16_to_cpu(ctxt->HashAlgorithmCount) != 1)
		printk_once(KERN_WARNING "illegal SMB3 hash algorithm count\n");
	if (ctxt->HashAlgorithms != SMB2_PREAUTH_INTEGRITY_SHA512)
		printk_once(KERN_WARNING "unknown SMB3 hash algorithm\n");
}