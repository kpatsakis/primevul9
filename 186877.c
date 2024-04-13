doshn(struct magic_set *ms, int clazz, int swap, int fd, off_t off, int num,
    size_t size, off_t fsize, int *flags, int mach, int strtab)
{
	Elf32_Shdr sh32;
	Elf64_Shdr sh64;
	int stripped = 1;
	void *nbuf;
	off_t noff, coff, name_off;
	uint64_t cap_hw1 = 0;	/* SunOS 5.x hardware capabilites */
	uint64_t cap_sf1 = 0;	/* SunOS 5.x software capabilites */
	char name[50];

	if (size != xsh_sizeof) {
		if (file_printf(ms, ", corrupted section header size") == -1)
			return -1;
		return 0;
	}

	for ( ; num; num--) {
		if (FINFO_LSEEK_FUNC(fd, off, SEEK_SET) == (off_t)-1) {
			file_badseek(ms);
			return -1;
		}
		if (FINFO_READ_FUNC(fd, xsh_addr, xsh_sizeof) == -1) {
			file_badread(ms);
			return -1;
		}
		off += size;

		/* Things we can determine before we seek */
		switch (xsh_type) {
		case SHT_SYMTAB:
#if 0
		case SHT_DYNSYM:
#endif
			stripped = 0;
			break;
		default:
			if (xsh_offset > fsize) {
				/* Perhaps warn here */
				continue;
			}
			break;
		}

		/* Things we can determine when we seek */
		switch (xsh_type) {
		case SHT_NOTE:
			nbuf = emalloc((size_t)xsh_size);
			if ((noff = FINFO_LSEEK_FUNC(fd, (off_t)xsh_offset, SEEK_SET)) ==
			    (off_t)-1) {
				file_badread(ms);
				efree(nbuf);
				return -1;
			}
			if (FINFO_READ_FUNC(fd, nbuf, (size_t)xsh_size) !=
			    (ssize_t)xsh_size) {
				efree(nbuf);
				file_badread(ms);
				return -1;
			}

			noff = 0;
			for (;;) {
				if (noff >= (off_t)xsh_size)
					break;
				noff = donote(ms, nbuf, (size_t)noff,
				    (size_t)xsh_size, clazz, swap, 4,
				    flags);
				if (noff == 0)
					break;
			}
			efree(nbuf);
			break;
		case SHT_SUNW_cap:
			if (FINFO_LSEEK_FUNC(fd, (off_t)xsh_offset, SEEK_SET) ==
			    (off_t)-1) {
				file_badseek(ms);
				return -1;
			}
			coff = 0;
			for (;;) {
				Elf32_Cap cap32;
				Elf64_Cap cap64;
				char cbuf[/*CONSTCOND*/
				    MAX(sizeof cap32, sizeof cap64)];
				if ((coff += xcap_sizeof) > (off_t)xsh_size)
					break;
				if (FINFO_READ_FUNC(fd, cbuf, (size_t)xcap_sizeof) !=
				    (ssize_t)xcap_sizeof) {
					file_badread(ms);
					return -1;
				}
				(void)memcpy(xcap_addr, cbuf, xcap_sizeof);
				switch (xcap_tag) {
				case CA_SUNW_NULL:
					break;
				case CA_SUNW_HW_1:
					cap_hw1 |= xcap_val;
					break;
				case CA_SUNW_SF_1:
					cap_sf1 |= xcap_val;
					break;
				default:
					if (file_printf(ms,
					    ", with unknown capability "
					    "0x%" INT64_T_FORMAT "x = 0x%"
					    INT64_T_FORMAT "x",
					    (unsigned long long)xcap_tag,
					    (unsigned long long)xcap_val) == -1)
						return -1;
					break;
				}
			}
			break;

		default:
			break;
		}
	}
	if (file_printf(ms, ", %sstripped", stripped ? "" : "not ") == -1)
		return -1;
	if (cap_hw1) {
		const cap_desc_t *cdp;
		switch (mach) {
		case EM_SPARC:
		case EM_SPARC32PLUS:
		case EM_SPARCV9:
			cdp = cap_desc_sparc;
			break;
		case EM_386:
		case EM_IA_64:
		case EM_AMD64:
			cdp = cap_desc_386;
			break;
		default:
			cdp = NULL;
			break;
		}
		if (file_printf(ms, ", uses") == -1)
			return -1;
		if (cdp) {
			while (cdp->cd_name) {
				if (cap_hw1 & cdp->cd_mask) {
					if (file_printf(ms,
					    " %s", cdp->cd_name) == -1)
						return -1;
					cap_hw1 &= ~cdp->cd_mask;
				}
				++cdp;
			}
			if (cap_hw1)
				if (file_printf(ms,
				    " unknown hardware capability 0x%"
				    INT64_T_FORMAT "x",
				    (unsigned long long)cap_hw1) == -1)
					return -1;
		} else {
			if (file_printf(ms,
			    " hardware capability 0x%" INT64_T_FORMAT "x",
			    (unsigned long long)cap_hw1) == -1)
				return -1;
		}
	}
	if (cap_sf1) {
		if (cap_sf1 & SF1_SUNW_FPUSED) {
			if (file_printf(ms,
			    (cap_sf1 & SF1_SUNW_FPKNWN)
			    ? ", uses frame pointer"
			    : ", not known to use frame pointer") == -1)
				return -1;
		}
		cap_sf1 &= ~SF1_SUNW_MASK;
		if (cap_sf1)
			if (file_printf(ms,
			    ", with unknown software capability 0x%"
			    INT64_T_FORMAT "x",
			    (unsigned long long)cap_sf1) == -1)
				return -1;
	}
	return 0;
}