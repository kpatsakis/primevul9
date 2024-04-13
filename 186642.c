ppt_read_atom_header(int fd, atom_header_t *atom_header)
{
	uint16_t v;
	struct ppt_header {
		uint16_t ver;
		uint16_t type;
		uint32_t length;
	} h;

	cli_dbgmsg("in ppt_read_atom_header\n");
	if(cli_readn(fd, &h, sizeof(struct ppt_header)) != sizeof(struct ppt_header)) {
		cli_dbgmsg("read ppt_header failed\n");
		return FALSE;
	}
	v = vba_endian_convert_16(h.ver, FALSE);
	cli_dbgmsg("\tversion: 0x%.2x\n", v & 0xF);
	cli_dbgmsg("\tinstance: 0x%.2x\n", v >> 4);

	atom_header->type = vba_endian_convert_16(h.type, FALSE);
	cli_dbgmsg("\ttype: 0x%.4x\n", atom_header->type);
	atom_header->length = vba_endian_convert_32(h.length, FALSE);
	cli_dbgmsg("\tlength: 0x%.8x\n", (int)atom_header->length);

	return TRUE;
}