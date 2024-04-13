int phar_tar_flush(phar_archive_data *phar, char *user_stub, zend_long len, int defaultstub, char **error) /* {{{ */
{
	phar_entry_info entry = {0};
	static const char newstub[] = "<?php // tar-based phar archive stub file\n__HALT_COMPILER();";
	php_stream *oldfile, *newfile, *stubfile;
	int closeoldfile, free_user_stub, signature_length;
	struct _phar_pass_tar_info pass;
	char *buf, *signature, *tmp, sigbuf[8];
	char halt_stub[] = "__HALT_COMPILER();";

	entry.flags = PHAR_ENT_PERM_DEF_FILE;
	entry.timestamp = time(NULL);
	entry.is_modified = 1;
	entry.is_crc_checked = 1;
	entry.is_tar = 1;
	entry.tar_type = '0';
	entry.phar = phar;
	entry.fp_type = PHAR_MOD;
	entry.fp = NULL;
	entry.filename = NULL;

	if (phar->is_persistent) {
		if (error) {
			spprintf(error, 0, "internal error: attempt to flush cached tar-based phar \"%s\"", phar->fname);
		}
		return EOF;
	}

	if (phar->is_data) {
		goto nostub;
	}

	/* set alias */
	if (!phar->is_temporary_alias && phar->alias_len) {
		entry.filename = estrndup(".phar/alias.txt", sizeof(".phar/alias.txt")-1);
		entry.filename_len = sizeof(".phar/alias.txt")-1;
		entry.fp = php_stream_fopen_tmpfile();
		if (entry.fp == NULL) {
			efree(entry.filename);
			spprintf(error, 0, "phar error: unable to create temporary file");
			return -1;
		}
		if (phar->alias_len != (int)php_stream_write(entry.fp, phar->alias, phar->alias_len)) {
			if (error) {
				spprintf(error, 0, "unable to set alias in tar-based phar \"%s\"", phar->fname);
			}
			php_stream_close(entry.fp);
			efree(entry.filename);
			return EOF;
		}

		entry.uncompressed_filesize = phar->alias_len;

		if (NULL == zend_hash_str_update_mem(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info))) {
			if (error) {
				spprintf(error, 0, "unable to set alias in tar-based phar \"%s\"", phar->fname);
			}
			php_stream_close(entry.fp);
			efree(entry.filename);
			return EOF;
		}
		/* At this point the entry is saved into the manifest. The manifest destroy
			routine will care about any resources to be freed. */
	} else {
		zend_hash_str_del(&phar->manifest, ".phar/alias.txt", sizeof(".phar/alias.txt")-1);
	}

	/* set stub */
	if (user_stub && !defaultstub) {
		char *pos;
		if (len < 0) {
			/* resource passed in */
			if (!(php_stream_from_zval_no_verify(stubfile, (zval *)user_stub))) {
				if (error) {
					spprintf(error, 0, "unable to access resource to copy stub to new tar-based phar \"%s\"", phar->fname);
				}
				return EOF;
			}
			if (len == -1) {
				len = PHP_STREAM_COPY_ALL;
			} else {
				len = -len;
			}
			user_stub = 0;

			// TODO: refactor to avoid reallocation ???
//???		len = php_stream_copy_to_mem(stubfile, &user_stub, len, 0)
			{
				zend_string *str = php_stream_copy_to_mem(stubfile, len, 0);
				if (str) {
					len = ZSTR_LEN(str);
					user_stub = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
					zend_string_release(str);
				} else {
					user_stub = NULL;
					len = 0;
				}
			}

			if (!len || !user_stub) {
				if (error) {
					spprintf(error, 0, "unable to read resource to copy stub to new tar-based phar \"%s\"", phar->fname);
				}
				return EOF;
			}
			free_user_stub = 1;
		} else {
			free_user_stub = 0;
		}

		tmp = estrndup(user_stub, len);
		if ((pos = php_stristr(tmp, halt_stub, len, sizeof(halt_stub) - 1)) == NULL) {
			efree(tmp);
			if (error) {
				spprintf(error, 0, "illegal stub for tar-based phar \"%s\"", phar->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			return EOF;
		}
		pos = user_stub + (pos - tmp);
		efree(tmp);

		len = pos - user_stub + 18;
		entry.fp = php_stream_fopen_tmpfile();
		if (entry.fp == NULL) {
			spprintf(error, 0, "phar error: unable to create temporary file");
			return EOF;
		}
		entry.uncompressed_filesize = len + 5;

		if ((size_t)len != php_stream_write(entry.fp, user_stub, len)
		||            5 != php_stream_write(entry.fp, " ?>\r\n", 5)) {
			if (error) {
				spprintf(error, 0, "unable to create stub from string in new tar-based phar \"%s\"", phar->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			php_stream_close(entry.fp);
			return EOF;
		}

		entry.filename = estrndup(".phar/stub.php", sizeof(".phar/stub.php")-1);
		entry.filename_len = sizeof(".phar/stub.php")-1;
		zend_hash_str_update_mem(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info));

		if (free_user_stub) {
			efree(user_stub);
		}
	} else {
		/* Either this is a brand new phar (add the stub), or the default stub is required (overwrite the stub) */
		entry.fp = php_stream_fopen_tmpfile();
		if (entry.fp == NULL) {
			spprintf(error, 0, "phar error: unable to create temporary file");
			return EOF;
		}
		if (sizeof(newstub)-1 != php_stream_write(entry.fp, newstub, sizeof(newstub)-1)) {
			php_stream_close(entry.fp);
			if (error) {
				spprintf(error, 0, "unable to %s stub in%star-based phar \"%s\", failed", user_stub ? "overwrite" : "create", user_stub ? " " : " new ", phar->fname);
			}
			return EOF;
		}

		entry.uncompressed_filesize = entry.compressed_filesize = sizeof(newstub) - 1;
		entry.filename = estrndup(".phar/stub.php", sizeof(".phar/stub.php")-1);
		entry.filename_len = sizeof(".phar/stub.php")-1;

		if (!defaultstub) {
			if (!zend_hash_str_exists(&phar->manifest, ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
				if (NULL == zend_hash_str_add_mem(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info))) {
					php_stream_close(entry.fp);
					efree(entry.filename);
					if (error) {
						spprintf(error, 0, "unable to create stub in tar-based phar \"%s\"", phar->fname);
					}
					return EOF;
				}
			} else {
				php_stream_close(entry.fp);
				efree(entry.filename);
			}
		} else {
			if (NULL == zend_hash_str_update_mem(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info))) {
				php_stream_close(entry.fp);
				efree(entry.filename);
				if (error) {
					spprintf(error, 0, "unable to overwrite stub in tar-based phar \"%s\"", phar->fname);
				}
				return EOF;
			}
		}
	}
nostub:
	if (phar->fp && !phar->is_brandnew) {
		oldfile = phar->fp;
		closeoldfile = 0;
		php_stream_rewind(oldfile);
	} else {
		oldfile = php_stream_open_wrapper(phar->fname, "rb", 0, NULL);
		closeoldfile = oldfile != NULL;
	}

	newfile = php_stream_fopen_tmpfile();
	if (!newfile) {
		if (error) {
			spprintf(error, 0, "unable to create temporary file");
		}
		if (closeoldfile) {
			php_stream_close(oldfile);
		}
		return EOF;
	}

	pass.old = oldfile;
	pass.new = newfile;
	pass.error = error;
	pass.free_fp = 1;
	pass.free_ufp = 1;

	if (Z_TYPE(phar->metadata) != IS_UNDEF) {
		phar_entry_info *mentry;
		if (NULL != (mentry = zend_hash_str_find_ptr(&(phar->manifest), ".phar/.metadata.bin", sizeof(".phar/.metadata.bin")-1))) {
			if (ZEND_HASH_APPLY_KEEP != phar_tar_setmetadata(&phar->metadata, mentry, error)) {
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				return EOF;
			}
		} else {
			phar_entry_info newentry = {0};

			newentry.filename = estrndup(".phar/.metadata.bin", sizeof(".phar/.metadata.bin")-1);
			newentry.filename_len = sizeof(".phar/.metadata.bin")-1;
			newentry.phar = phar;
			newentry.tar_type = TAR_FILE;
			newentry.is_tar = 1;

			if (NULL == (mentry = zend_hash_str_add_mem(&(phar->manifest), ".phar/.metadata.bin", sizeof(".phar/.metadata.bin")-1, (void *)&newentry, sizeof(phar_entry_info)))) {
				spprintf(error, 0, "phar tar error: unable to add magic metadata file to manifest for phar archive \"%s\"", phar->fname);
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				return EOF;
			}

			if (ZEND_HASH_APPLY_KEEP != phar_tar_setmetadata(&phar->metadata, mentry, error)) {
				zend_hash_str_del(&(phar->manifest), ".phar/.metadata.bin", sizeof(".phar/.metadata.bin")-1);
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				return EOF;
			}
		}
	}

	zend_hash_apply_with_argument(&phar->manifest, phar_tar_setupmetadata, (void *) &pass);

	if (error && *error) {
		if (closeoldfile) {
			php_stream_close(oldfile);
		}

		/* on error in the hash iterator above, error is set */
		php_stream_close(newfile);
		return EOF;
	}

	zend_hash_apply_with_argument(&phar->manifest, phar_tar_writeheaders, (void *) &pass);

	/* add signature for executable tars or tars explicitly set with setSignatureAlgorithm */
	if (!phar->is_data || phar->sig_flags) {
		if (FAILURE == phar_create_signature(phar, newfile, &signature, &signature_length, error)) {
			if (error) {
				char *save = *error;
				spprintf(error, 0, "phar error: unable to write signature to tar-based phar: %s", save);
				efree(save);
			}

			if (closeoldfile) {
				php_stream_close(oldfile);
			}

			php_stream_close(newfile);
			return EOF;
		}

		entry.filename = ".phar/signature.bin";
		entry.filename_len = sizeof(".phar/signature.bin")-1;
		entry.fp = php_stream_fopen_tmpfile();
		if (entry.fp == NULL) {
			spprintf(error, 0, "phar error: unable to create temporary file");
			return EOF;
		}
#ifdef WORDS_BIGENDIAN
# define PHAR_SET_32(var, buffer) \
	*(uint32_t *)(var) = (((((unsigned char*)&(buffer))[3]) << 24) \
		| ((((unsigned char*)&(buffer))[2]) << 16) \
		| ((((unsigned char*)&(buffer))[1]) << 8) \
		| (((unsigned char*)&(buffer))[0]))
#else
# define PHAR_SET_32(var, buffer) *(uint32_t *)(var) = (uint32_t) (buffer)
#endif
		PHAR_SET_32(sigbuf, phar->sig_flags);
		PHAR_SET_32(sigbuf + 4, signature_length);

		if (8 != (int)php_stream_write(entry.fp, sigbuf, 8) || signature_length != (int)php_stream_write(entry.fp, signature, signature_length)) {
			efree(signature);
			if (error) {
				spprintf(error, 0, "phar error: unable to write signature to tar-based phar %s", phar->fname);
			}

			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			return EOF;
		}

		efree(signature);
		entry.uncompressed_filesize = entry.compressed_filesize = signature_length + 8;
		/* throw out return value and write the signature */
		entry.filename_len = phar_tar_writeheaders_int(&entry, (void *)&pass);

		if (error && *error) {
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			/* error is set by writeheaders */
			php_stream_close(newfile);
			return EOF;
		}
	} /* signature */

	/* add final zero blocks */
	buf = (char *) ecalloc(1024, 1);
	php_stream_write(newfile, buf, 1024);
	efree(buf);

	if (closeoldfile) {
		php_stream_close(oldfile);
	}

	/* on error in the hash iterator above, error is set */
	if (error && *error) {
		php_stream_close(newfile);
		return EOF;
	}

	if (phar->fp && pass.free_fp) {
		php_stream_close(phar->fp);
	}

	if (phar->ufp) {
		if (pass.free_ufp) {
			php_stream_close(phar->ufp);
		}
		phar->ufp = NULL;
	}

	phar->is_brandnew = 0;
	php_stream_rewind(newfile);

	if (phar->donotflush) {
		/* deferred flush */
		phar->fp = newfile;
	} else {
		phar->fp = php_stream_open_wrapper(phar->fname, "w+b", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);
		if (!phar->fp) {
			phar->fp = newfile;
			if (error) {
				spprintf(error, 0, "unable to open new phar \"%s\" for writing", phar->fname);
			}
			return EOF;
		}

		if (phar->flags & PHAR_FILE_COMPRESSED_GZ) {
			php_stream_filter *filter;
			/* to properly compress, we have to tell zlib to add a zlib header */
			zval filterparams;

			array_init(&filterparams);
/* this is defined in zlib's zconf.h */
#ifndef MAX_WBITS
#define MAX_WBITS 15
#endif
			add_assoc_long(&filterparams, "window", MAX_WBITS + 16);
			filter = php_stream_filter_create("zlib.deflate", &filterparams, php_stream_is_persistent(phar->fp));
			zval_dtor(&filterparams);

			if (!filter) {
				/* copy contents uncompressed rather than lose them */
				php_stream_copy_to_stream_ex(newfile, phar->fp, PHP_STREAM_COPY_ALL, NULL);
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 4096, "unable to compress all contents of phar \"%s\" using zlib, PHP versions older than 5.2.6 have a buggy zlib", phar->fname);
				}
				return EOF;
			}

			php_stream_filter_append(&phar->fp->writefilters, filter);
			php_stream_copy_to_stream_ex(newfile, phar->fp, PHP_STREAM_COPY_ALL, NULL);
			php_stream_filter_flush(filter, 1);
			php_stream_filter_remove(filter, 1);
			php_stream_close(phar->fp);
			/* use the temp stream as our base */
			phar->fp = newfile;
		} else if (phar->flags & PHAR_FILE_COMPRESSED_BZ2) {
			php_stream_filter *filter;

			filter = php_stream_filter_create("bzip2.compress", NULL, php_stream_is_persistent(phar->fp));
			php_stream_filter_append(&phar->fp->writefilters, filter);
			php_stream_copy_to_stream_ex(newfile, phar->fp, PHP_STREAM_COPY_ALL, NULL);
			php_stream_filter_flush(filter, 1);
			php_stream_filter_remove(filter, 1);
			php_stream_close(phar->fp);
			/* use the temp stream as our base */
			phar->fp = newfile;
		} else {
			php_stream_copy_to_stream_ex(newfile, phar->fp, PHP_STREAM_COPY_ALL, NULL);
			/* we could also reopen the file in "rb" mode but there is no need for that */
			php_stream_close(newfile);
		}
	}
	return EOF;
}