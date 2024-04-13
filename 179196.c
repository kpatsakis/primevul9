parse_options(const char *data, struct parsed_mount_info *parsed_info)
{
	char *value = NULL;
	char *equals = NULL;
	char *next_keyword = NULL;
	char *out = parsed_info->options;
	unsigned long *filesys_flags = &parsed_info->flags;
	int out_len = 0;
	int word_len;
	int rc = 0;
	int got_bkupuid = 0;
	int got_bkupgid = 0;
	int got_uid = 0;
	int got_cruid = 0;
	int got_gid = 0;
	int got_snapshot = 0;
	uid_t uid, cruid = 0, bkupuid = 0;
	gid_t gid, bkupgid = 0;
	char *ep;
	struct passwd *pw;
	struct group *gr;
	/*
	 * max 64-bit uint in decimal is 18446744073709551615 which is 20 chars
	 * wide +1 for NULL, and +1 for good measure
	 */
	char txtbuf[22];
	unsigned long long snapshot;
	struct tm tm;

	/* make sure we're starting from beginning */
	out[0] = '\0';

	/* BB fixme check for separator override BB */
	uid = getuid();
	if (uid != 0)
		got_uid = 1;

	gid = getgid();
	if (gid != 0)
		got_gid = 1;

	if (!data)
		return EX_USAGE;

	/*
	 * format is keyword,keyword2=value2,keyword3=value3... 
	 * data  = next keyword
	 * value = next value ie stuff after equal sign
	 */
	while (data && *data) {
		next_keyword = strchr(data, ',');	/* BB handle sep= */

		/* temporarily null terminate end of keyword=value pair */
		if (next_keyword)
			*next_keyword++ = 0;

		/* temporarily null terminate keyword if there's a value */
		value = NULL;
		if ((equals = strchr(data, '=')) != NULL) {
			*equals = '\0';
			value = equals + 1;
		}

		switch(parse_opt_token(data)) {
		case OPT_USERS:
			if (!value || !*value) {
				*filesys_flags |= MS_USERS;
				goto nocopy;
			}
			break;

		case OPT_USER:
			if (!value || !*value) {
				if (data[4] == '\0') {
					*filesys_flags |= MS_USER;
					goto nocopy;
				} else {
					fprintf(stderr,
						"username specified with no parameter\n");
					return EX_USAGE;
				}
			} else {
				strlcpy(parsed_info->username, value,
					sizeof(parsed_info->username));
				parsed_info->got_user = 1;
				goto nocopy;
			}

		case OPT_PASS:
			if (parsed_info->got_password) {
				fprintf(stderr,
					"password specified twice, ignoring second\n");
				goto nocopy;
			}
			if (!value || !*value) {
				parsed_info->got_password = 1;
				goto nocopy;
			}
			rc = set_password(parsed_info, value);
			if (rc)
				return rc;
			goto nocopy;

		case OPT_SEC:
			if (value) {
				if (!strncmp(value, "none", 4))
					parsed_info->got_password = 1;
				if (!strncmp(value, "krb5", 4)) {
					parsed_info->is_krb5 = 1;
					parsed_info->got_password = 1;
				}
			}
			break;

		case OPT_IP:
			if (!value || !*value) {
				fprintf(stderr,
					"target ip address argument missing\n");
			} else if (strnlen(value, MAX_ADDRESS_LEN) <
				MAX_ADDRESS_LEN) {
				strlcpy(parsed_info->addrlist, value,
					MAX_ADDRESS_LEN);
				if (parsed_info->verboseflag)
					fprintf(stderr,
						"ip address %s override specified\n",
						value);
				goto nocopy;
			} else {
				fprintf(stderr, "ip address too long\n");
				return EX_USAGE;

			}
			break;

		/* unc || target || path */
		case OPT_UNC:
			if (!value || !*value) {
				fprintf(stderr,
					"invalid path to network resource\n");
				return EX_USAGE;
			}
			rc = parse_unc(value, parsed_info, thisprogram);
			if (rc)
				return rc;
			break;

		/* dom || workgroup */
		case OPT_DOM:
			if (!value) {
				/*
				 * An empty domain has been passed
				 */
				/* not necessary but better safe than.. */
				parsed_info->domain[0] = '\0';
				parsed_info->got_domain = 1;
				goto nocopy;
			}
			if (strnlen(value, sizeof(parsed_info->domain)) >=
			    sizeof(parsed_info->domain)) {
				fprintf(stderr, "domain name too long\n");
				return EX_USAGE;
			}
			strlcpy(parsed_info->domain, value,
				sizeof(parsed_info->domain));
			goto nocopy;

		case OPT_CRED:
			if (!value || !*value) {
				fprintf(stderr,
					"invalid credential file name specified\n");
				return EX_USAGE;
			}
			rc = open_cred_file(value, parsed_info);
			if (rc) {
				fprintf(stderr,
					"error %d (%s) opening credential file %s\n",
					rc, strerror(rc), value);
				return rc;
			}
			goto nocopy;

		case OPT_UID:
			if (!value || !*value)
				goto nocopy;

			got_uid = 1;
			pw = getpwnam(value);
			if (pw) {
				uid = pw->pw_uid;
				goto nocopy;
			}

			errno = 0;
			uid = strtoul(value, &ep, 10);
			if (errno == 0 && *ep == '\0')
				goto nocopy;

			fprintf(stderr, "bad option uid=\"%s\"\n", value);
			return EX_USAGE;
		case OPT_CRUID:
			if (!value || !*value)
				goto nocopy;

			got_cruid = 1;
			pw = getpwnam(value);
			if (pw) {
				cruid = pw->pw_uid;
				goto nocopy;
			}

			errno = 0;
			cruid = strtoul(value, &ep, 10);
			if (errno == 0 && *ep == '\0')
				goto nocopy;

			fprintf(stderr, "bad option: cruid=\"%s\"\n", value);
			return EX_USAGE;
		case OPT_GID:
			if (!value || !*value)
				goto nocopy;

			got_gid = 1;
			gr = getgrnam(value);
			if (gr) {
				gid = gr->gr_gid;
				goto nocopy;
			}

			errno = 0;
			gid = strtoul(value, &ep, 10);
			if (errno == 0 && *ep == '\0')
				goto nocopy;

			fprintf(stderr, "bad option: gid=\"%s\"\n", value);
			return EX_USAGE;
		/* fmask falls through to file_mode */
		case OPT_FMASK:
			fprintf(stderr,
				"WARNING: CIFS mount option 'fmask' is\
				 deprecated. Use 'file_mode' instead.\n");
			data = "file_mode";	/* BB fix this */
			/* Fallthrough */
		case OPT_FILE_MODE:
			if (!value || !*value) {
				fprintf(stderr,
					"Option '%s' requires a numerical argument\n",
					data);
				return EX_USAGE;
			}

			if (value[0] != '0')
				fprintf(stderr,
					"WARNING: '%s' not expressed in octal.\n",
					data);
			break;

		/* dmask falls through to dir_mode */
		case OPT_DMASK:
			fprintf(stderr,
				"WARNING: CIFS mount option 'dmask' is\
				 deprecated. Use 'dir_mode' instead.\n");
			data = "dir_mode";
			/* Fallthrough */
		case OPT_DIR_MODE:
			if (!value || !*value) {
				fprintf(stderr,
					"Option '%s' requires a numerical argument\n",
					data);
				return EX_USAGE;
			}

			if (value[0] != '0')
				fprintf(stderr,
					"WARNING: '%s' not expressed in octal.\n",
					data);
			break;
		case OPT_NO_SUID:
			*filesys_flags |= MS_NOSUID;
			goto nocopy;
		case OPT_SUID:
			*filesys_flags &= ~MS_NOSUID;
			goto nocopy;
		case OPT_NO_DEV:
			*filesys_flags |= MS_NODEV;
			goto nocopy;
		case OPT_NO_LOCK:
			*filesys_flags &= ~MS_MANDLOCK;
			break;
		case OPT_MAND:
			*filesys_flags |= MS_MANDLOCK;
			goto nocopy;
		case OPT_NOMAND:
			*filesys_flags &= ~MS_MANDLOCK;
			goto nocopy;
		case OPT_DEV:
			*filesys_flags &= ~MS_NODEV;
			goto nocopy;
		case OPT_NO_EXEC:
			*filesys_flags |= MS_NOEXEC;
			goto nocopy;
		case OPT_EXEC:
			*filesys_flags &= ~MS_NOEXEC;
			goto nocopy;
		case OPT_GUEST:
			parsed_info->got_user = 1;
			parsed_info->got_password = 1;
			goto nocopy;
		case OPT_RO:
			*filesys_flags |= MS_RDONLY;
			goto nocopy;
		case OPT_RW:
			*filesys_flags &= ~MS_RDONLY;
			goto nocopy;
		case OPT_REMOUNT:
			*filesys_flags |= MS_REMOUNT;
			goto nocopy;
		case OPT_IGNORE:
			goto nocopy;
		case OPT_BKUPUID:
			if (!value || !*value)
				goto nocopy;

			got_bkupuid = 1;
			errno = 0;
			bkupuid = strtoul(value, &ep, 10);
			if (errno == 0 && *ep == '\0')
				goto nocopy;

			pw = getpwnam(value);
			if (pw == NULL) {
				fprintf(stderr,
					"bad user name \"%s\"\n", value);
				return EX_USAGE;
			}

			bkupuid = pw->pw_uid;
			goto nocopy;
		case OPT_BKUPGID:
			if (!value || !*value)
				goto nocopy;

			got_bkupgid = 1;
			errno = 0;
			bkupgid = strtoul(value, &ep, 10);
			if (errno == 0 && *ep == '\0')
				goto nocopy;

			gr = getgrnam(value);
			if (gr == NULL) {
				fprintf(stderr,
					"bad group name \"%s\"\n", value);
				return EX_USAGE;
			}

			bkupgid = gr->gr_gid;
			goto nocopy;
		case OPT_NOFAIL:
			parsed_info->nofail = 1;
			goto nocopy;
		case OPT_SNAPSHOT:
			if (!value || !*value)
				goto nocopy;
			if (strncmp(value, "@GMT-", 5))
				break;
			if ((strlen(value) != GMT_NAME_LEN) ||
			    (strptime(value, GMT_FORMAT, &tm) == NULL)) {
				fprintf(stderr, "bad snapshot token\n");
				return EX_USAGE;
			}
			snapshot = timegm(&tm) * 10000000 + NTFS_TIME_OFFSET;
			got_snapshot = 1;
			goto nocopy;
		}

		/* check size before copying option to buffer */
		word_len = strlen(data);
		if (value)
			word_len += 1 + strlen(value);

		/* need 2 extra bytes for comma and null byte */
		if (out_len + word_len + 2 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		/* put back equals sign, if any */
		if (equals)
			*equals = '=';

		/* go ahead and copy */
		if (out_len)
			strlcat(out, ",", MAX_OPTIONS_LEN);

		strlcat(out, data, MAX_OPTIONS_LEN);
		out_len = strlen(out);
nocopy:
		data = next_keyword;
	}


	/* special-case the uid and gid */
	if (got_uid) {
		word_len = snprintf(txtbuf, sizeof(txtbuf), "%u", uid);

		/* comma + "uid=" + terminating NULL == 6 */
		if (out_len + word_len + 6 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		if (out_len) {
			strlcat(out, ",", MAX_OPTIONS_LEN);
			out_len++;
		}
		snprintf(out + out_len, word_len + 5, "uid=%s", txtbuf);
		out_len = strlen(out);
	}
	if (parsed_info->is_krb5 && parsed_info->sudo_uid) {
		cruid = parsed_info->sudo_uid;
		got_cruid = 1;
	}
	if (got_cruid) {
		word_len = snprintf(txtbuf, sizeof(txtbuf), "%u", cruid);

		/* comma + "cruid=" + terminating NULL == 8 */
		if (out_len + word_len + 8 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		if (out_len) {
			strlcat(out, ",", MAX_OPTIONS_LEN);
			out_len++;
		}
		snprintf(out + out_len, word_len + 7, "cruid=%s", txtbuf);
		out_len = strlen(out);
	}
	if (got_gid) {
		word_len = snprintf(txtbuf, sizeof(txtbuf), "%u", gid);

		/* comma + "gid=" + terminating NULL == 6 */
		if (out_len + word_len + 6 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		if (out_len) {
			strlcat(out, ",", MAX_OPTIONS_LEN);
			out_len++;
		}
		snprintf(out + out_len, word_len + 5, "gid=%s", txtbuf);
		out_len = strlen(out);
	}
	if (got_bkupuid) {
		word_len = snprintf(txtbuf, sizeof(txtbuf), "%u", bkupuid);

		/* comma + "backupuid=" + terminating NULL == 12 */
		if (out_len + word_len + 12 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		if (out_len) {
			strlcat(out, ",", MAX_OPTIONS_LEN);
			out_len++;
		}
		snprintf(out + out_len, word_len + 11, "backupuid=%s", txtbuf);
		out_len = strlen(out);
	}
	if (got_bkupgid) {
		word_len = snprintf(txtbuf, sizeof(txtbuf), "%u", bkupgid);

		/* comma + "backupgid=" + terminating NULL == 12 */
		if (out_len + word_len + 12 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		if (out_len) {
			strlcat(out, ",", MAX_OPTIONS_LEN);
			out_len++;
		}
		snprintf(out + out_len, word_len + 11, "backupgid=%s", txtbuf);
		out_len = strlen(out);
	}
	if (got_snapshot) {
		word_len = snprintf(txtbuf, sizeof(txtbuf), "%llu", snapshot);

		/* comma + "snapshot=" + terminating NULL == 11 */
		if (out_len + word_len + 11 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		if (out_len) {
			strlcat(out, ",", MAX_OPTIONS_LEN);
			out_len++;
		}
		snprintf(out + out_len, word_len + 10, "snapshot=%s", txtbuf);
		out_len = strlen(out);
	}

	return 0;
}