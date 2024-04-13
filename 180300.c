_zip_dirent_init(struct zip_dirent *de)
{
    de->version_madeby = 0;
    de->version_needed = 20; /* 2.0 */
    de->bitflags = 0;
    de->comp_method = 0;
    de->last_mod = 0;
    de->crc = 0;
    de->comp_size = 0;
    de->uncomp_size = 0;
    de->filename = NULL;
    de->filename_len = 0;
    de->extrafield = NULL;
    de->extrafield_len = 0;
    de->comment = NULL;
    de->comment_len = 0;
    de->disk_number = 0;
    de->int_attrib = 0;
    de->ext_attrib = 0;
    de->offset = 0;
}