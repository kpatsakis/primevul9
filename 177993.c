phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len, char **error, int security) /* {{{ */
{
	return phar_get_entry_info_dir(phar, path, path_len, 0, error, security);
}