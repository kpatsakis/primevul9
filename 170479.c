struct MACH0_(mach_header) * MACH0_(get_hdr)(RzBuffer *buf) {
	ut8 magicbytes[sizeof(ut32)] = { 0 };
	ut8 machohdrbytes[sizeof(struct MACH0_(mach_header))] = { 0 };
	int len;
	struct MACH0_(mach_header) *macho_hdr = RZ_NEW0(struct MACH0_(mach_header));
	bool big_endian = false;
	if (!macho_hdr) {
		return NULL;
	}
	if (rz_buf_read_at(buf, 0, magicbytes, 4) < 1) {
		free(macho_hdr);
		return false;
	}

	if (rz_read_le32(magicbytes) == 0xfeedface) {
		big_endian = false;
	} else if (rz_read_be32(magicbytes) == 0xfeedface) {
		big_endian = true;
	} else if (rz_read_le32(magicbytes) == FAT_MAGIC) {
		big_endian = false;
	} else if (rz_read_be32(magicbytes) == FAT_MAGIC) {
		big_endian = true;
	} else if (rz_read_le32(magicbytes) == 0xfeedfacf) {
		big_endian = false;
	} else if (rz_read_be32(magicbytes) == 0xfeedfacf) {
		big_endian = true;
	} else {
		/* also extract non-mach0s */
#if 0
		free (macho_hdr);
		return NULL;
#endif
	}
	len = rz_buf_read_at(buf, 0, machohdrbytes, sizeof(machohdrbytes));
	if (len != sizeof(struct MACH0_(mach_header))) {
		free(macho_hdr);
		return NULL;
	}
	macho_hdr->magic = rz_read_ble(&machohdrbytes[0], big_endian, 32);
	macho_hdr->cputype = rz_read_ble(&machohdrbytes[4], big_endian, 32);
	macho_hdr->cpusubtype = rz_read_ble(&machohdrbytes[8], big_endian, 32);
	macho_hdr->filetype = rz_read_ble(&machohdrbytes[12], big_endian, 32);
	macho_hdr->ncmds = rz_read_ble(&machohdrbytes[16], big_endian, 32);
	macho_hdr->sizeofcmds = rz_read_ble(&machohdrbytes[20], big_endian, 32);
	macho_hdr->flags = rz_read_ble(&machohdrbytes[24], big_endian, 32);
#if RZ_BIN_MACH064
	macho_hdr->reserved = rz_read_ble(&machohdrbytes[28], big_endian, 32);
#endif
	return macho_hdr;
}