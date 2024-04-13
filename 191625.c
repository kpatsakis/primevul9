_zip_dirent_init(zip_dirent_t *de)
{
    de->changed = 0;
    de->local_extra_fields_read = 0;
    de->cloned = 0;

    de->crc_valid = true;
    de->version_madeby = 20 | (ZIP_OPSYS_DEFAULT << 8);
    de->version_needed = 20; /* 2.0 */
    de->bitflags = 0;
    de->comp_method = ZIP_CM_DEFAULT;
    de->last_mod = 0;
    de->crc = 0;
    de->comp_size = 0;
    de->uncomp_size = 0;
    de->filename = NULL;
    de->extra_fields = NULL;
    de->comment = NULL;
    de->disk_number = 0;
    de->int_attrib = 0;
    de->ext_attrib = ZIP_EXT_ATTRIB_DEFAULT;
    de->offset = 0;
    de->encryption_method = ZIP_EM_NONE;
}