cli_vba_readdir(const char *dir, struct uniq *U, uint32_t which)
{
	unsigned char *buf;
	const unsigned char vba56_signature[] = { 0xcc, 0x61 };
	uint16_t record_count, buflen, ffff, byte_count;
	uint32_t offset;
	int i, j, fd, big_endian = FALSE;
	vba_project_t *vba_project;
	struct vba56_header v56h;
	off_t seekback;
	char fullname[1024], *hash;

	cli_dbgmsg("in cli_vba_readdir()\n");

	if(dir == NULL)
		return NULL;

	/*
	 * _VBA_PROJECT files are embedded within office documents (OLE2)
	 */
	
	if (!uniq_get(U, "_vba_project", 12, &hash))
		return NULL;
	snprintf(fullname, sizeof(fullname), "%s"PATHSEP"%s_%u", dir, hash, which);
	fullname[sizeof(fullname)-1] = '\0';
	fd = open(fullname, O_RDONLY|O_BINARY);

	if(fd == -1)
		return NULL;

	if(cli_readn(fd, &v56h, sizeof(struct vba56_header)) != sizeof(struct vba56_header)) {
		close(fd);
		return NULL;
	}
	if (memcmp(v56h.magic, vba56_signature, sizeof(v56h.magic)) != 0) {
		close(fd);
		return NULL;
	}

	i = vba_read_project_strings(fd, TRUE);
	seekback = lseek(fd, 0, SEEK_CUR);
	if (lseek(fd, sizeof(struct vba56_header), SEEK_SET) == -1) {
		close(fd);
		return NULL;
	}
	j = vba_read_project_strings(fd, FALSE);
	if(!i && !j) {
		close(fd);
		cli_dbgmsg("vba_readdir: Unable to guess VBA type\n");
		return NULL;
	}
	if (i > j) {
		big_endian = TRUE;
		lseek(fd, seekback, SEEK_SET);
		cli_dbgmsg("vba_readdir: Guessing big-endian\n");
	} else {
		cli_dbgmsg("vba_readdir: Guessing little-endian\n");
	}

	/* junk some more stuff */
	do
		if (cli_readn(fd, &ffff, 2) != 2) {
			close(fd);
			return NULL;
		}
	while(ffff != 0xFFFF);

	/* check for alignment error */
	if(!seekandread(fd, -3, SEEK_CUR, &ffff, sizeof(uint16_t))) {
		close(fd);
		return NULL;
	}
	if (ffff != 0xFFFF)
		lseek(fd, 1, SEEK_CUR);

	if(!read_uint16(fd, &ffff, big_endian)) {
		close(fd);
		return NULL;
	}

	if(ffff != 0xFFFF)
		lseek(fd, ffff, SEEK_CUR);

	if(!read_uint16(fd, &ffff, big_endian)) {
		close(fd);
		return NULL;
	}

	if(ffff == 0xFFFF)
		ffff = 0;

	lseek(fd, ffff + 100, SEEK_CUR);

	if(!read_uint16(fd, &record_count, big_endian)) {
		close(fd);
		return NULL;
	}
	cli_dbgmsg("vba_readdir: VBA Record count %d\n", record_count);
	if (record_count == 0) {
		/* No macros, assume clean */
		close(fd);
		return NULL;
	}
	if (record_count > MAX_VBA_COUNT) {
		/* Almost certainly an error */
		cli_dbgmsg("vba_readdir: VBA Record count too big\n");
		close(fd);
		return NULL;
	}

	vba_project = create_vba_project(record_count, dir, U);
	if(vba_project == NULL) {
		close(fd);
		return NULL;
	}
	buf = NULL;
	buflen = 0;
	for(i = 0; i < record_count; i++) {
		uint16_t length;
		char *ptr;

		vba_project->colls[i] = 0;
		if(!read_uint16(fd, &length, big_endian))
			break;

		if (length == 0) {
			cli_dbgmsg("vba_readdir: zero name length\n");
			break;
		}
		if(length > buflen) {
			unsigned char *newbuf = (unsigned char *)cli_realloc(buf, length);
			if(newbuf == NULL)
				break;
			buflen = length;
			buf = newbuf;
		}
		if (cli_readn(fd, buf, length) != length) {
			cli_dbgmsg("vba_readdir: read name failed\n");
			break;
		}
		ptr = get_unicode_name((const char *)buf, length, big_endian);
		if(ptr == NULL) break;
		if (!(vba_project->colls[i]=uniq_get(U, ptr, strlen(ptr), &hash))) {
			cli_dbgmsg("vba_readdir: cannot find project %s (%s)\n", ptr, hash);
			break;
		}
		cli_dbgmsg("vba_readdir: project name: %s (%s)\n", ptr, hash);
		free(ptr);
		vba_project->name[i] = hash;
		if(!read_uint16(fd, &length, big_endian))
			break;
		lseek(fd, length, SEEK_CUR);

		if(!read_uint16(fd, &ffff, big_endian))
			break;
		if (ffff == 0xFFFF) {
			lseek(fd, 2, SEEK_CUR);
			if(!read_uint16(fd, &ffff, big_endian))
				break;
			lseek(fd, ffff + 8, SEEK_CUR);
		} else
			lseek(fd, ffff + 10, SEEK_CUR);

		if(!read_uint16(fd, &byte_count, big_endian))
			break;
		lseek(fd, (8 * byte_count) + 5, SEEK_CUR);
		if(!read_uint32(fd, &offset, big_endian))
			break;
		cli_dbgmsg("vba_readdir: offset: %u\n", (unsigned int)offset);
		vba_project->offset[i] = offset;
		lseek(fd, 2, SEEK_CUR);
	}

	if(buf)
		free(buf);

	close(fd);

	if(i < record_count) {
		free(vba_project->name);
		free(vba_project->colls);
		free(vba_project->dir);
		free(vba_project->offset);
		free(vba_project);
		return NULL;
	}

	return vba_project;
}