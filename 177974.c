static int phar_zip_applysignature(phar_archive_data *phar, struct _phar_zip_pass *pass,
				   smart_str *metadata) /* {{{ */
{
	/* add signature for executable tars or tars explicitly set with setSignatureAlgorithm */
	if (!phar->is_data || phar->sig_flags) {
		int signature_length;
		char *signature, sigbuf[8];
		phar_entry_info entry = {0};
		php_stream *newfile;
		zend_off_t tell, st;

		newfile = php_stream_fopen_tmpfile();
		if (newfile == NULL) {
			spprintf(pass->error, 0, "phar error: unable to create temporary file for the signature file");
			return FAILURE;
		}
		st = tell = php_stream_tell(pass->filefp);
		/* copy the local files, central directory, and the zip comment to generate the hash */
		php_stream_seek(pass->filefp, 0, SEEK_SET);
		php_stream_copy_to_stream_ex(pass->filefp, newfile, tell, NULL);
		tell = php_stream_tell(pass->centralfp);
		php_stream_seek(pass->centralfp, 0, SEEK_SET);
		php_stream_copy_to_stream_ex(pass->centralfp, newfile, tell, NULL);
		if (metadata->s) {
			php_stream_write(newfile, ZSTR_VAL(metadata->s), ZSTR_LEN(metadata->s));
		}

		if (FAILURE == phar_create_signature(phar, newfile, &signature, &signature_length, pass->error)) {
			if (pass->error) {
				char *save = *(pass->error);
				spprintf(pass->error, 0, "phar error: unable to write signature to zip-based phar: %s", save);
				efree(save);
			}

			php_stream_close(newfile);
			return FAILURE;
		}

		entry.filename = ".phar/signature.bin";
		entry.filename_len = sizeof(".phar/signature.bin")-1;
		entry.fp = php_stream_fopen_tmpfile();
		entry.fp_type = PHAR_MOD;
		entry.is_modified = 1;
		if (entry.fp == NULL) {
			spprintf(pass->error, 0, "phar error: unable to create temporary file for signature");
			return FAILURE;
		}

		PHAR_SET_32(sigbuf, phar->sig_flags);
		PHAR_SET_32(sigbuf + 4, signature_length);

		if (8 != (int)php_stream_write(entry.fp, sigbuf, 8) || signature_length != (int)php_stream_write(entry.fp, signature, signature_length)) {
			efree(signature);
			if (pass->error) {
				spprintf(pass->error, 0, "phar error: unable to write signature to zip-based phar %s", phar->fname);
			}

			php_stream_close(newfile);
			return FAILURE;
		}

		efree(signature);
		entry.uncompressed_filesize = entry.compressed_filesize = signature_length + 8;
		entry.phar = phar;
		/* throw out return value and write the signature */
		phar_zip_changed_apply_int(&entry, (void *)pass);
		php_stream_close(newfile);

		if (pass->error && *(pass->error)) {
			/* error is set by writeheaders */
			php_stream_close(newfile);
			return FAILURE;
		}
	} /* signature */
	return SUCCESS;
}