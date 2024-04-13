static int php_stdiop_set_option(php_stream *stream, int option, int value, void *ptrparam)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*) stream->abstract;
	size_t size;
	int fd;
#ifdef O_NONBLOCK
	/* FIXME: make this work for win32 */
	int flags;
	int oldval;
#endif

	PHP_STDIOP_GET_FD(fd, data);

	switch(option) {
		case PHP_STREAM_OPTION_BLOCKING:
			if (fd == -1)
				return -1;
#ifdef O_NONBLOCK
			flags = fcntl(fd, F_GETFL, 0);
			oldval = (flags & O_NONBLOCK) ? 0 : 1;
			if (value)
				flags &= ~O_NONBLOCK;
			else
				flags |= O_NONBLOCK;

			if (-1 == fcntl(fd, F_SETFL, flags))
				return -1;
			return oldval;
#else
			return -1; /* not yet implemented */
#endif

		case PHP_STREAM_OPTION_WRITE_BUFFER:

			if (data->file == NULL) {
				return -1;
			}

			if (ptrparam)
				size = *(size_t *)ptrparam;
			else
				size = BUFSIZ;

			switch(value) {
				case PHP_STREAM_BUFFER_NONE:
					return setvbuf(data->file, NULL, _IONBF, 0);

				case PHP_STREAM_BUFFER_LINE:
					return setvbuf(data->file, NULL, _IOLBF, size);

				case PHP_STREAM_BUFFER_FULL:
					return setvbuf(data->file, NULL, _IOFBF, size);

				default:
					return -1;
			}
			break;

		case PHP_STREAM_OPTION_LOCKING:
			if (fd == -1) {
				return -1;
			}

			if ((zend_uintptr_t) ptrparam == PHP_STREAM_LOCK_SUPPORTED) {
				return 0;
			}

			if (!flock(fd, value)) {
				data->lock_flag = value;
				return 0;
			} else {
				return -1;
			}
			break;

		case PHP_STREAM_OPTION_MMAP_API:
#if HAVE_MMAP
			{
				php_stream_mmap_range *range = (php_stream_mmap_range*)ptrparam;
				int prot, flags;

				switch (value) {
					case PHP_STREAM_MMAP_SUPPORTED:
						return fd == -1 ? PHP_STREAM_OPTION_RETURN_ERR : PHP_STREAM_OPTION_RETURN_OK;

					case PHP_STREAM_MMAP_MAP_RANGE:
						if(do_fstat(data, 1) != 0) {
							return PHP_STREAM_OPTION_RETURN_ERR;
						}
						if (range->length == 0 && range->offset > 0 && range->offset < data->sb.st_size) {
							range->length = data->sb.st_size - range->offset;
						}
						if (range->length == 0 || range->length > data->sb.st_size) {
							range->length = data->sb.st_size;
						}
						if (range->offset >= data->sb.st_size) {
							range->offset = data->sb.st_size;
							range->length = 0;
						}
						switch (range->mode) {
							case PHP_STREAM_MAP_MODE_READONLY:
								prot = PROT_READ;
								flags = MAP_PRIVATE;
								break;
							case PHP_STREAM_MAP_MODE_READWRITE:
								prot = PROT_READ | PROT_WRITE;
								flags = MAP_PRIVATE;
								break;
							case PHP_STREAM_MAP_MODE_SHARED_READONLY:
								prot = PROT_READ;
								flags = MAP_SHARED;
								break;
							case PHP_STREAM_MAP_MODE_SHARED_READWRITE:
								prot = PROT_READ | PROT_WRITE;
								flags = MAP_SHARED;
								break;
							default:
								return PHP_STREAM_OPTION_RETURN_ERR;
						}
						range->mapped = (char*)mmap(NULL, range->length, prot, flags, fd, range->offset);
						if (range->mapped == (char*)MAP_FAILED) {
							range->mapped = NULL;
							return PHP_STREAM_OPTION_RETURN_ERR;
						}
						/* remember the mapping */
						data->last_mapped_addr = range->mapped;
						data->last_mapped_len = range->length;
						return PHP_STREAM_OPTION_RETURN_OK;

					case PHP_STREAM_MMAP_UNMAP:
						if (data->last_mapped_addr) {
							munmap(data->last_mapped_addr, data->last_mapped_len);
							data->last_mapped_addr = NULL;

							return PHP_STREAM_OPTION_RETURN_OK;
						}
						return PHP_STREAM_OPTION_RETURN_ERR;
				}
			}
#elif defined(PHP_WIN32)
			{
				php_stream_mmap_range *range = (php_stream_mmap_range*)ptrparam;
				HANDLE hfile = (HANDLE)_get_osfhandle(fd);
				DWORD prot, acc, loffs = 0, delta = 0;

				switch (value) {
					case PHP_STREAM_MMAP_SUPPORTED:
						return hfile == INVALID_HANDLE_VALUE ? PHP_STREAM_OPTION_RETURN_ERR : PHP_STREAM_OPTION_RETURN_OK;

					case PHP_STREAM_MMAP_MAP_RANGE:
						switch (range->mode) {
							case PHP_STREAM_MAP_MODE_READONLY:
								prot = PAGE_READONLY;
								acc = FILE_MAP_READ;
								break;
							case PHP_STREAM_MAP_MODE_READWRITE:
								prot = PAGE_READWRITE;
								acc = FILE_MAP_READ | FILE_MAP_WRITE;
								break;
							case PHP_STREAM_MAP_MODE_SHARED_READONLY:
								prot = PAGE_READONLY;
								acc = FILE_MAP_READ;
								/* TODO: we should assign a name for the mapping */
								break;
							case PHP_STREAM_MAP_MODE_SHARED_READWRITE:
								prot = PAGE_READWRITE;
								acc = FILE_MAP_READ | FILE_MAP_WRITE;
								/* TODO: we should assign a name for the mapping */
								break;
							default:
								return PHP_STREAM_OPTION_RETURN_ERR;
						}

						/* create a mapping capable of viewing the whole file (this costs no real resources) */
						data->file_mapping = CreateFileMapping(hfile, NULL, prot, 0, 0, NULL);

						if (data->file_mapping == NULL) {
							return PHP_STREAM_OPTION_RETURN_ERR;
						}

						size = GetFileSize(hfile, NULL);
						if (range->length == 0 && range->offset > 0 && range->offset < size) {
							range->length = size - range->offset;
						}
						if (range->length == 0 || range->length > size) {
							range->length = size;
						}
						if (range->offset >= size) {
							range->offset = size;
							range->length = 0;
						}

						/* figure out how big a chunk to map to be able to view the part that we need */
						if (range->offset != 0) {
							SYSTEM_INFO info;
							DWORD gran;

							GetSystemInfo(&info);
							gran = info.dwAllocationGranularity;
							loffs = ((DWORD)range->offset / gran) * gran;
							delta = (DWORD)range->offset - loffs;
						}

						data->last_mapped_addr = MapViewOfFile(data->file_mapping, acc, 0, loffs, range->length + delta);

						if (data->last_mapped_addr) {
							/* give them back the address of the start offset they requested */
							range->mapped = data->last_mapped_addr + delta;
							return PHP_STREAM_OPTION_RETURN_OK;
						}

						CloseHandle(data->file_mapping);
						data->file_mapping = NULL;

						return PHP_STREAM_OPTION_RETURN_ERR;

					case PHP_STREAM_MMAP_UNMAP:
						if (data->last_mapped_addr) {
							UnmapViewOfFile(data->last_mapped_addr);
							data->last_mapped_addr = NULL;
							CloseHandle(data->file_mapping);
							data->file_mapping = NULL;
							return PHP_STREAM_OPTION_RETURN_OK;
						}
						return PHP_STREAM_OPTION_RETURN_ERR;

					default:
						return PHP_STREAM_OPTION_RETURN_ERR;
				}
			}

#endif
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;

		case PHP_STREAM_OPTION_TRUNCATE_API:
			switch (value) {
				case PHP_STREAM_TRUNCATE_SUPPORTED:
					return fd == -1 ? PHP_STREAM_OPTION_RETURN_ERR : PHP_STREAM_OPTION_RETURN_OK;

				case PHP_STREAM_TRUNCATE_SET_SIZE: {
					ptrdiff_t new_size = *(ptrdiff_t*)ptrparam;
					if (new_size < 0) {
						return PHP_STREAM_OPTION_RETURN_ERR;
					}
					return ftruncate(fd, new_size) == 0 ? PHP_STREAM_OPTION_RETURN_OK : PHP_STREAM_OPTION_RETURN_ERR;
				}
			}

#ifdef PHP_WIN32
		case PHP_STREAM_OPTION_PIPE_BLOCKING:
			data->is_pipe_blocking = value;
			return PHP_STREAM_OPTION_RETURN_OK;
#endif
		case PHP_STREAM_OPTION_META_DATA_API:
			if (fd == -1)
				return -1;
#ifdef O_NONBLOCK
			flags = fcntl(fd, F_GETFL, 0);

			add_assoc_bool((zval*)ptrparam, "timed_out", 0);
			add_assoc_bool((zval*)ptrparam, "blocked", (flags & O_NONBLOCK)? 0 : 1);
			add_assoc_bool((zval*)ptrparam, "eof", stream->eof);

			return PHP_STREAM_OPTION_RETURN_OK;
#endif
			return -1;
		default:
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;
	}
}