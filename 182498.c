SMB2_query_acl(const unsigned int xid, struct cifs_tcon *tcon,
		u64 persistent_fid, u64 volatile_fid,
		void **data, u32 *plen)
{
	__u32 additional_info = OWNER_SECINFO | GROUP_SECINFO | DACL_SECINFO;
	*plen = 0;

	return query_info(xid, tcon, persistent_fid, volatile_fid,
			  0, SMB2_O_INFO_SECURITY, additional_info,
			  SMB2_MAX_BUFFER_SIZE, MIN_SEC_DESC_LEN, data, plen);
}