static void phar_add_file(phar_archive_data **pphar, char *filename, int filename_len, char *cont_str, int cont_len, zval *zresource TSRMLS_DC)
{
	char *error;
	size_t contents_len;
	phar_entry_data *data;
	php_stream *contents_file;

	if (filename_len >= sizeof(".phar")-1 && !memcmp(filename, ".phar", sizeof(".phar")-1) && (filename[5] == '/' || filename[5] == '\\' || filename[5] == '\0')) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot create any files in magic \".phar\" directory", (*pphar)->fname);
		return;
	}

	if (!(data = phar_get_or_create_entry_data((*pphar)->fname, (*pphar)->fname_len, filename, filename_len, "w+b", 0, &error, 1 TSRMLS_CC))) {
		if (error) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist and cannot be created: %s", filename, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist and cannot be created", filename);
		}
		return;
	} else {
		if (error) {
			efree(error);
		}

		if (!data->internal_file->is_dir) {
			if (cont_str) {
				contents_len = php_stream_write(data->fp, cont_str, cont_len);
				if (contents_len != cont_len) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s could not be written to", filename);
					return;
				}
			} else {
				if (!(php_stream_from_zval_no_verify(contents_file, &zresource))) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s could not be written to", filename);
					return;
				}
				phar_stream_copy_to_stream(contents_file, data->fp, PHP_STREAM_COPY_ALL, &contents_len);
			}

			data->internal_file->compressed_filesize = data->internal_file->uncompressed_filesize = contents_len;
		}

		/* check for copy-on-write */
		if (pphar[0] != data->phar) {
			*pphar = data->phar;
		}
		phar_entry_delref(data TSRMLS_CC);
		phar_flush(*pphar, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
	}
}