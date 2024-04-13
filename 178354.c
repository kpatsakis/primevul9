int finish_transfer(const char *fname, const char *fnametmp,
		    const char *fnamecmp, const char *partialptr,
		    struct file_struct *file, int ok_to_set_time,
		    int overwriting_basis)
{
	int ret;
	const char *temp_copy_name = partialptr && *partialptr != '/' ? partialptr : NULL;

	if (inplace) {
		if (DEBUG_GTE(RECV, 1))
			rprintf(FINFO, "finishing %s\n", fname);
		fnametmp = fname;
		goto do_set_file_attrs;
	}

	if (make_backups > 0 && overwriting_basis) {
		int ok = make_backup(fname, False);
		if (!ok)
			exit_cleanup(RERR_FILEIO);
		if (ok == 1 && fnamecmp == fname)
			fnamecmp = get_backup_name(fname);
	}

	/* Change permissions before putting the file into place. */
	set_file_attrs(fnametmp, file, NULL, fnamecmp,
		       ok_to_set_time ? 0 : ATTRS_SKIP_MTIME);

	/* move tmp file over real file */
	if (DEBUG_GTE(RECV, 1))
		rprintf(FINFO, "renaming %s to %s\n", fnametmp, fname);
	ret = robust_rename(fnametmp, fname, temp_copy_name, file->mode);
	if (ret < 0) {
		rsyserr(FERROR_XFER, errno, "%s %s -> \"%s\"",
			ret == -2 ? "copy" : "rename",
			full_fname(fnametmp), fname);
		if (!partialptr || (ret == -2 && temp_copy_name)
		 || robust_rename(fnametmp, partialptr, NULL, file->mode) < 0)
			do_unlink(fnametmp);
		return 0;
	}
	if (ret == 0) {
		/* The file was moved into place (not copied), so it's done. */
		return 1;
	}
	/* The file was copied, so tweak the perms of the copied file.  If it
	 * was copied to partialptr, move it into its final destination. */
	fnametmp = temp_copy_name ? temp_copy_name : fname;

  do_set_file_attrs:
	set_file_attrs(fnametmp, file, NULL, fnamecmp,
		       ok_to_set_time ? 0 : ATTRS_SKIP_MTIME);

	if (temp_copy_name) {
		if (do_rename(fnametmp, fname) < 0) {
			rsyserr(FERROR_XFER, errno, "rename %s -> \"%s\"",
				full_fname(fnametmp), fname);
			return 0;
		}
		handle_partial_dir(temp_copy_name, PDIR_DELETE);
	}
	return 1;
}