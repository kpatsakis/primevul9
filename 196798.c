zzip_mem_entry_new(ZZIP_DISK* disk, ZZIP_DISK_ENTRY* entry) 
{
    if (! disk || ! entry) { errno=EINVAL; return 0; }
    ___ ZZIP_MEM_ENTRY* item = calloc(1, sizeof(*item));
    if (! item) return 0; /* errno=ENOMEM; */
    ___ struct zzip_file_header* header = 
	zzip_disk_entry_to_file_header(disk, entry);
    /*  there is a number of duplicated information in the file header
     *  or the disk entry block. Theoretically some part may be missing
     *  that exists in the other, ... but we will prefer the disk entry.
     */
    item->zz_comment =   zzip_disk_entry_strdup_comment(disk, entry);
    item->zz_name =      zzip_disk_entry_strdup_name(disk, entry);
    item->zz_data =      zzip_file_header_to_data(header);
    item->zz_flags =     zzip_disk_entry_get_flags(entry);
    item->zz_compr =     zzip_disk_entry_get_compr(entry);
    item->zz_crc32 =     zzip_disk_entry_get_crc32(entry);
    item->zz_csize =     zzip_disk_entry_get_csize(entry);
    item->zz_usize =     zzip_disk_entry_get_usize(entry);
    item->zz_diskstart = zzip_disk_entry_get_diskstart(entry);
    item->zz_filetype =  zzip_disk_entry_get_filetype(entry);

    {   /* copy the extra blocks to memory as well */
	int     ext1 = zzip_disk_entry_get_extras(entry);
	char* R ptr1 = zzip_disk_entry_to_extras(entry);
	int     ext2 = zzip_file_header_get_extras(header);
	char* R ptr2 = zzip_file_header_to_extras(header);
	
	if (ext1) {
	    void* mem = malloc (ext1+2);
	    item->zz_ext[1] = mem;
	    memcpy (mem, ptr1, ext1);
	    ((char*)(mem))[ext1+0] = 0; 
	    ((char*)(mem))[ext1+1] = 0;
	}	    
	if (ext2) {
	    void* mem = malloc (ext2+2);
	    item->zz_ext[2] = mem;
	    memcpy (mem, ptr2, ext2);
	    ((char*)(mem))[ext2+0] = 0; 
	    ((char*)(mem))[ext2+1] = 0;
	}	    
    }    
    {   /* override sizes/offsets with zip64 values for largefile support */
	zzip_extra_zip64* block = (zzip_extra_zip64*)
	    zzip_mem_entry_extra_block (item, ZZIP_EXTRA_zip64);
	if (block) {
	    item->zz_usize  =    __zzip_get64(block->z_usize);
	    item->zz_csize  =    __zzip_get64(block->z_csize);
	    item->zz_offset =    __zzip_get64(block->z_offset);
	    item->zz_diskstart = __zzip_get32(block->z_diskstart);
	}
    }
    /* NOTE: 
     * All information from the central directory entry is now in memory.
     * Effectivly that allows us to modify it and write it back to disk.
     */
    return item; ____;____;
}