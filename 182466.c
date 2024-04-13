fill_small_buf(__le16 smb2_command, struct cifs_tcon *tcon, void *buf,
	       unsigned int *total_len)
{
	struct smb2_sync_pdu *spdu = (struct smb2_sync_pdu *)buf;
	/* lookup word count ie StructureSize from table */
	__u16 parmsize = smb2_req_struct_sizes[le16_to_cpu(smb2_command)];

	/*
	 * smaller than SMALL_BUFFER_SIZE but bigger than fixed area of
	 * largest operations (Create)
	 */
	memset(buf, 0, 256);

	smb2_hdr_assemble(&spdu->sync_hdr, smb2_command, tcon);
	spdu->StructureSize2 = cpu_to_le16(parmsize);

	*total_len = parmsize + sizeof(struct smb2_sync_hdr);
}