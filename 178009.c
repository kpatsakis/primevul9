char * phar_decompress_filter(phar_entry_info * entry, int return_unknown) /* {{{ */
{
	php_uint32 flags;

	if (entry->is_modified) {
		flags = entry->old_flags;
	} else {
		flags = entry->flags;
	}

	switch (flags & PHAR_ENT_COMPRESSION_MASK) {
		case PHAR_ENT_COMPRESSED_GZ:
			return "zlib.inflate";
		case PHAR_ENT_COMPRESSED_BZ2:
			return "bzip2.decompress";
		default:
			return return_unknown ? "unknown" : NULL;
	}
}