static bool valid_smb_header(const uint8_t *inbuf)
{
	if (is_encrypted_packet(inbuf)) {
		return true;
	}
	/*
	 * This used to be (strncmp(smb_base(inbuf),"\377SMB",4) == 0)
	 * but it just looks weird to call strncmp for this one.
	 */
	return (IVAL(smb_base(inbuf), 0) == 0x424D53FF);
}