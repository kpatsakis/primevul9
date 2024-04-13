static int calculate_open_access_flags(uint32_t access_mask,
				       int oplock_request,
				       uint32_t private_flags)
{
	bool need_write, need_read;

	/*
	 * Note that we ignore the append flag as append does not
	 * mean the same thing under DOS and Unix.
	 */

	need_write = (access_mask & (FILE_WRITE_DATA | FILE_APPEND_DATA));
	if (!need_write) {
		return O_RDONLY;
	}

	/* DENY_DOS opens are always underlying read-write on the
	   file handle, no matter what the requested access mask
	   says. */

	need_read =
		((private_flags & NTCREATEX_OPTIONS_PRIVATE_DENY_DOS) ||
		 access_mask & (FILE_READ_ATTRIBUTES|FILE_READ_DATA|
				FILE_READ_EA|FILE_EXECUTE));

	if (!need_read) {
		return O_WRONLY;
	}
	return O_RDWR;
}