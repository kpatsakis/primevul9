NTSTATUS fd_open(struct connection_struct *conn,
		 files_struct *fsp,
		 int flags,
		 mode_t mode)
{
	struct smb_filename *smb_fname = fsp->fsp_name;
	NTSTATUS status = NT_STATUS_OK;

#ifdef O_NOFOLLOW
	/* 
	 * Never follow symlinks on a POSIX client. The
	 * client should be doing this.
	 */

	if (fsp->posix_open || !lp_symlinks(SNUM(conn))) {
		flags |= O_NOFOLLOW;
	}
#endif

	fsp->fh->fd = SMB_VFS_OPEN(conn, smb_fname, fsp, flags, mode);
	if (fsp->fh->fd == -1) {
		int posix_errno = errno;
#ifdef O_NOFOLLOW
#if defined(ENOTSUP) && defined(OSF1)
		/* handle special Tru64 errno */
		if (errno == ENOTSUP) {
			posix_errno = ELOOP;
		}
#endif /* ENOTSUP */
#ifdef EFTYPE
		/* fix broken NetBSD errno */
		if (errno == EFTYPE) {
			posix_errno = ELOOP;
		}
#endif /* EFTYPE */
		/* fix broken FreeBSD errno */
		if (errno == EMLINK) {
			posix_errno = ELOOP;
		}
#endif /* O_NOFOLLOW */
		status = map_nt_error_from_unix(posix_errno);
		if (errno == EMFILE) {
			static time_t last_warned = 0L;

			if (time((time_t *) NULL) > last_warned) {
				DEBUG(0,("Too many open files, unable "
					"to open more!  smbd's max "
					"open files = %d\n",
					lp_max_open_files()));
				last_warned = time((time_t *) NULL);
			}
		}

	}

	DEBUG(10,("fd_open: name %s, flags = 0%o mode = 0%o, fd = %d. %s\n",
		  smb_fname_str_dbg(smb_fname), flags, (int)mode, fsp->fh->fd,
		(fsp->fh->fd == -1) ? strerror(errno) : "" ));

	return status;
}