word_read_macro_entry(int fd, macro_info_t *macro_info)
{
	int msize;
	int count = macro_info->count;
	macro_entry_t *macro_entry;
#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif

#ifdef HAVE_PRAGMA_PACK_HPPA
#pragma pack 1
#endif
	struct macro {
		unsigned char version;
		unsigned char key;
		unsigned char ignore[10];
		uint32_t len	__attribute__ ((packed));
		uint32_t state	__attribute__ ((packed));
		uint32_t offset	__attribute__ ((packed));
	} *m;
	const struct macro *n;
#ifdef HAVE_PRAGMA_PACK
#pragma pack()
#endif

#ifdef HAVE_PRAGMA_PACK_HPPA
#pragma pack
#endif
	if(count == 0)
		return TRUE;

	msize = count * sizeof(struct macro);
	m = cli_malloc(msize);
	if(m == NULL)
		return FALSE;

	if(cli_readn(fd, m, msize) != msize) {
		free(m);
		cli_warnmsg("read %d macro_entries failed\n", count);
		return FALSE;
	}
	macro_entry = macro_info->entries;
	n = m;
	do {
		macro_entry->key = n->key;
		macro_entry->len = vba_endian_convert_32(n->len, FALSE);
		macro_entry->offset = vba_endian_convert_32(n->offset, FALSE);
		macro_entry++;
		n++;
	} while(--count > 0);
	free(m);
	return TRUE;
}