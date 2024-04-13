static bool parent_override_delete(connection_struct *conn,
					const struct smb_filename *smb_fname,
					uint32_t access_mask,
					uint32_t rejected_mask)
{
	if ((access_mask & DELETE_ACCESS) &&
		    (rejected_mask & DELETE_ACCESS) &&
		    can_delete_file_in_directory(conn, smb_fname)) {
		return true;
	}
	return false;
}