ppt_stream_iter(int fd, const char *dir)
{
	atom_header_t atom_header;

	while(ppt_read_atom_header(fd, &atom_header)) {
		if(atom_header.length == 0)
			return NULL;

		if(atom_header.type == 0x1011) {
			uint32_t length;

			/* Skip over ID */
			if(lseek(fd, sizeof(uint32_t), SEEK_CUR) == -1) {
				cli_dbgmsg("ppt_stream_iter: seek failed\n");
				return NULL;
			}
			length = atom_header.length - 4;
			cli_dbgmsg("length: %d\n", (int)length);
			if (!ppt_unlzw(dir, fd, length)) {
				cli_dbgmsg("ppt_unlzw failed\n");
				return NULL;
			}
		} else {
			off_t offset = lseek(fd, 0, SEEK_CUR);
			/* Check we don't wrap */
			if ((offset + (off_t)atom_header.length) < offset) {
				break;
			}
			offset += atom_header.length;
			if (lseek(fd, offset, SEEK_SET) != offset) {
				break;
			}
		}
	}
	return dir;
}