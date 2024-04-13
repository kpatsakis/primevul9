zzip_mem_entry_find_extra_block(ZZIP_MEM_ENTRY * entry, short datatype, zzip_size_t blocksize)
{
    int i = 2;
    while (1)
    {
        char* ext = (char*)( entry->zz_ext[i] );
        char* ext_end = ext + entry->zz_extlen[i];
        if (ext)
        {
	    /*
	     * Make sure that
	     * 1) the extra block header
	     * AND
	     * 2) the block we're looking for
	     * fit into the extra block!
	     */
            while (ext + zzip_extra_block_headerlength + blocksize <= ext_end)
            {
                if (datatype == zzip_extra_block_get_datatype(ext))
                {
                    if (blocksize <= zzip_extra_block_get_datasize(ext) + zzip_extra_block_headerlength)
                    {
                        return ((ZZIP_EXTRA_BLOCK*) ext);
                    }
                }
                /* skip to start of next extra_block */
                ___ zzip_size_t datasize = zzip_extra_block_get_datasize(ext);
                ext += zzip_extra_block_headerlength;
                ext += datasize;
                ____;
            }
        }
        if (! i)
            return 0;
        i--;
    }
}