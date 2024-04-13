static int php_stdiop_close(php_stream *stream, int close_handle)
{
	int ret;
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);

#if HAVE_MMAP
	if (data->last_mapped_addr) {
		munmap(data->last_mapped_addr, data->last_mapped_len);
		data->last_mapped_addr = NULL;
	}
#elif defined(PHP_WIN32)
	if (data->last_mapped_addr) {
		UnmapViewOfFile(data->last_mapped_addr);
		data->last_mapped_addr = NULL;
	}
	if (data->file_mapping) {
		CloseHandle(data->file_mapping);
		data->file_mapping = NULL;
	}
#endif

	if (close_handle) {
		if (data->file) {
			if (data->is_process_pipe) {
				errno = 0;
				ret = pclose(data->file);

#if HAVE_SYS_WAIT_H
				if (WIFEXITED(ret)) {
					ret = WEXITSTATUS(ret);
				}
#endif
			} else {
				ret = fclose(data->file);
				data->file = NULL;
			}
		} else if (data->fd != -1) {
			ret = close(data->fd);
			data->fd = -1;
		} else {
			return 0; /* everything should be closed already -> success */
		}
		if (data->temp_name) {
#ifdef PHP_WIN32
			php_win32_ioutil_unlink(ZSTR_VAL(data->temp_name));
#else
			unlink(ZSTR_VAL(data->temp_name));
#endif
			/* temporary streams are never persistent */
			zend_string_release(data->temp_name);
			data->temp_name = NULL;
		}
	} else {
		ret = 0;
		data->file = NULL;
		data->fd = -1;
	}

	pefree(data, stream->is_persistent);

	return ret;
}