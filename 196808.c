zzip_mem_entry_extra_block (ZZIP_MEM_ENTRY* entry, short datatype)
{
    int i = 2;
    while (1) {
	ZZIP_EXTRA_BLOCK* ext = entry->zz_ext[i];
	if (ext) {
	    while (ext->z_datatype) {
		if (datatype == zzip_extra_block_get_datatype (ext)) {
		    return ext;
		}
		___ char* e = (char*) ext;
		e += zzip_extra_block_headerlength;
		e += zzip_extra_block_get_datasize (ext);
		ext = (void*) e; ____;
	    }
	}
	if (! i) return 0;
	i--;
    }
}