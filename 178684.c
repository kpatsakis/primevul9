static inline void spl_filesystem_object_get_file_name(spl_filesystem_object *intern TSRMLS_DC) /* {{{ */
{
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			if (!intern->file_name) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Object not initialized");
			}
			break;
		case SPL_FS_DIR:
			if (intern->file_name) {
				efree(intern->file_name);
			}
			intern->file_name_len = spprintf(&intern->file_name, 0, "%s%c%s",
			                                 spl_filesystem_object_get_path(intern, NULL TSRMLS_CC),
			                                 slash, intern->u.dir.entry.d_name);
			break;
	}
} /* }}} */