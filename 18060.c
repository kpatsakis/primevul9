NTSTATUS smbd_calculate_access_mask(connection_struct *conn,
				    const struct smb_filename *smb_fname,
				    bool use_privs,
				    uint32_t access_mask,
				    uint32_t *access_mask_out)
{
	NTSTATUS status;
	uint32_t orig_access_mask = access_mask;
	uint32_t rejected_share_access;

	/*
	 * Convert GENERIC bits to specific bits.
	 */

	se_map_generic(&access_mask, &file_generic_mapping);

	/* Calculate MAXIMUM_ALLOWED_ACCESS if requested. */
	if (access_mask & MAXIMUM_ALLOWED_ACCESS) {

		status = smbd_calculate_maximum_allowed_access(
			conn, smb_fname, use_privs, &access_mask);

		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}

		access_mask &= conn->share_access;
	}

	rejected_share_access = access_mask & ~(conn->share_access);

	if (rejected_share_access) {
		DEBUG(10, ("smbd_calculate_access_mask: Access denied on "
			"file %s: rejected by share access mask[0x%08X] "
			"orig[0x%08X] mapped[0x%08X] reject[0x%08X]\n",
			smb_fname_str_dbg(smb_fname),
			conn->share_access,
			orig_access_mask, access_mask,
			rejected_share_access));
		return NT_STATUS_ACCESS_DENIED;
	}

	*access_mask_out = access_mask;
	return NT_STATUS_OK;
}