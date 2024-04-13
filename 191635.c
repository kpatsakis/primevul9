_zip_dirent_needs_zip64(const zip_dirent_t *de, zip_flags_t flags)
{
    if (de->uncomp_size >= ZIP_UINT32_MAX || de->comp_size >= ZIP_UINT32_MAX
	|| ((flags & ZIP_FL_CENTRAL) && de->offset >= ZIP_UINT32_MAX))
	return true;

    return false;
}