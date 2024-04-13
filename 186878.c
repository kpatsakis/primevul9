dophn_exec(struct magic_set *ms, int clazz, int swap, int fd, off_t off,
    int num, size_t size, off_t fsize, int *flags, int sh_num)
{
	Elf32_Phdr ph32;
	Elf64_Phdr ph64;
	const char *linking_style = "statically";
	const char *shared_libraries = "";
	unsigned char nbuf[BUFSIZ];
	ssize_t bufsize;
	size_t offset, align;
	
	if (size != xph_sizeof) {
		if (file_printf(ms, ", corrupted program header size") == -1)
			return -1;
		return 0;
	}

  	for ( ; num; num--) {
		if (FINFO_LSEEK_FUNC(fd, off, SEEK_SET) == (off_t)-1) {
			file_badseek(ms);
			return -1;
		}

  		if (FINFO_READ_FUNC(fd, xph_addr, xph_sizeof) == -1) {
  			file_badread(ms);
			return -1;
		}

		off += size;

		/* Things we can determine before we seek */
		switch (xph_type) {
		case PT_DYNAMIC:
			linking_style = "dynamically";
			break;
		case PT_INTERP:
			shared_libraries = " (uses shared libs)";
			break;
		default:
			if (xph_offset > fsize) {
				/* Maybe warn here? */
				continue;
			}
			break;
		}

		/* Things we can determine when we seek */
		switch (xph_type) {
		case PT_NOTE:
			if ((align = xph_align) & 0x80000000UL) {
				if (file_printf(ms, 
				    ", invalid note alignment 0x%lx",
				    (unsigned long)align) == -1)
					return -1;
				align = 4;
			}
			if (sh_num)
				break;
			/*
			 * This is a PT_NOTE section; loop through all the notes
			 * in the section.
			 */
			if (FINFO_LSEEK_FUNC(fd, xph_offset, SEEK_SET) == (off_t)-1) {
				file_badseek(ms);
				return -1;
			}
			bufsize = FINFO_READ_FUNC(fd, nbuf, ((xph_filesz < sizeof(nbuf)) ?
			    xph_filesz : sizeof(nbuf)));
			if (bufsize == -1) {
				file_badread(ms);
				return -1;
			}
			offset = 0;
			for (;;) {
				if (offset >= (size_t)bufsize)
					break;
				offset = donote(ms, nbuf, offset,
				    (size_t)bufsize, clazz, swap, align,
				    flags);
				if (offset == 0)
					break;
			}
			break;
		default:
			break;
		}
	}
	if (file_printf(ms, ", %s linked%s", linking_style, shared_libraries)
	    == -1)
	    return -1;
	return 0;
}