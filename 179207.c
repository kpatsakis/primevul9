del_mtab(char *mountpoint)
{
	int len, tmprc, rc = 0;
	FILE *mnttmp, *mntmtab;
	struct mntent *mountent;
	char *mtabfile, *mtabdir, *mtabtmpfile = NULL;

	mtabfile = strdup(MOUNTED);
	if (!mtabfile) {
		fprintf(stderr, "del_mtab: cannot strdup MOUNTED\n");
		rc = EX_FILEIO;
		goto del_mtab_exit;
	}

	mtabdir = dirname(mtabfile);
	len = strlen(mtabdir) + strlen(MNT_TMP_FILE);
	mtabtmpfile = malloc(len + 1);
	if (!mtabtmpfile) {
		fprintf(stderr, "del_mtab: cannot allocate memory to tmp file\n");
		rc = EX_FILEIO;
		goto del_mtab_exit;
	}

	if (sprintf(mtabtmpfile, "%s%s", mtabdir, MNT_TMP_FILE) != len) {
		fprintf(stderr, "del_mtab: error writing new string\n");
		rc = EX_FILEIO;
		goto del_mtab_exit;
	}

	atexit(unlock_mtab);
	rc = lock_mtab();
	if (rc) {
		fprintf(stderr, "del_mtab: cannot lock mtab\n");
		rc = EX_FILEIO;
		goto del_mtab_exit;
	}

	mtabtmpfile = mktemp(mtabtmpfile);
	if (!mtabtmpfile) {
		fprintf(stderr, "del_mtab: cannot setup tmp file destination\n");
		rc = EX_FILEIO;
		goto del_mtab_exit;
	}

	mntmtab = setmntent(MOUNTED, "r");
	if (!mntmtab) {
		fprintf(stderr, "del_mtab: could not update mount table\n");
		rc = EX_FILEIO;
		goto del_mtab_exit;
	}

	mnttmp = setmntent(mtabtmpfile, "w");
	if (!mnttmp) {
		fprintf(stderr, "del_mtab: could not update mount table\n");
		endmntent(mntmtab);
		rc = EX_FILEIO;
		goto del_mtab_exit;
	}

	while ((mountent = getmntent(mntmtab)) != NULL) {
		if (!strcmp(mountent->mnt_dir, mountpoint))
			continue;
		rc = addmntent(mnttmp, mountent);
		if (rc) {
			fprintf(stderr, "del_mtab: unable to add mount entry to mtab\n");
			rc = EX_FILEIO;
			goto del_mtab_error;
		}
	}

	endmntent(mntmtab);

	tmprc = my_endmntent(mnttmp, 0);
	if (tmprc) {
		fprintf(stderr, "del_mtab: error %d detected on close of tmp file\n", tmprc);
		rc = EX_FILEIO;
		goto del_mtab_error;
	}

	if (rename(mtabtmpfile, MOUNTED)) {
		fprintf(stderr, "del_mtab: error %d when renaming mtab in place\n", errno);
		rc = EX_FILEIO;
		goto del_mtab_error;
	}

del_mtab_exit:
	unlock_mtab();
	free(mtabtmpfile);
	free(mtabfile);
	return rc;

del_mtab_error:
	if (unlink(mtabtmpfile))
		fprintf(stderr, "del_mtab: failed to delete tmp file - %s\n",
				strerror(errno));
	goto del_mtab_exit;
}