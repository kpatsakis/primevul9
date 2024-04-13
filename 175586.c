evbuffer_file_segment_materialize(struct evbuffer_file_segment *seg)
{
	const unsigned flags = seg->flags;
	const int fd = seg->fd;
	const ev_off_t length = seg->length;
	const ev_off_t offset = seg->file_offset;

	if (seg->contents)
		return 0; /* already materialized */

#if defined(EVENT__HAVE_MMAP)
	if (!(flags & EVBUF_FS_DISABLE_MMAP)) {
		off_t offset_rounded = 0, offset_leftover = 0;
		void *mapped;
		if (offset) {
			/* mmap implementations don't generally like us
			 * to have an offset that isn't a round  */
			long page_size = get_page_size();
			if (page_size == -1)
				goto err;
			offset_leftover = offset % page_size;
			offset_rounded = offset - offset_leftover;
		}
		mapped = mmap(NULL, length + offset_leftover,
		    PROT_READ,
#ifdef MAP_NOCACHE
		    MAP_NOCACHE | /* ??? */
#endif
#ifdef MAP_FILE
		    MAP_FILE |
#endif
		    MAP_PRIVATE,
		    fd, offset_rounded);
		if (mapped == MAP_FAILED) {
			event_warn("%s: mmap(%d, %d, %zu) failed",
			    __func__, fd, 0, (size_t)(offset + length));
		} else {
			seg->mapping = mapped;
			seg->contents = (char*)mapped+offset_leftover;
			seg->mmap_offset = 0;
			seg->is_mapping = 1;
			goto done;
		}
	}
#endif
#ifdef _WIN32
	if (!(flags & EVBUF_FS_DISABLE_MMAP)) {
		intptr_t h = _get_osfhandle(fd);
		HANDLE m;
		ev_uint64_t total_size = length+offset;
		if ((HANDLE)h == INVALID_HANDLE_VALUE)
			goto err;
		m = CreateFileMapping((HANDLE)h, NULL, PAGE_READONLY,
		    (total_size >> 32), total_size & 0xfffffffful,
		    NULL);
		if (m != INVALID_HANDLE_VALUE) { /* Does h leak? */
			seg->mapping_handle = m;
			seg->mmap_offset = offset;
			seg->is_mapping = 1;
			goto done;
		}
	}
#endif
	{
		ev_off_t start_pos = lseek(fd, 0, SEEK_CUR), pos;
		ev_off_t read_so_far = 0;
		char *mem;
		int e;
		ev_ssize_t n = 0;
		if (!(mem = mm_malloc(length)))
			goto err;
		if (start_pos < 0) {
			mm_free(mem);
			goto err;
		}
		if (lseek(fd, offset, SEEK_SET) < 0) {
			mm_free(mem);
			goto err;
		}
		while (read_so_far < length) {
			n = read(fd, mem+read_so_far, length-read_so_far);
			if (n <= 0)
				break;
			read_so_far += n;
		}

		e = errno;
		pos = lseek(fd, start_pos, SEEK_SET);
		if (n < 0 || (n == 0 && length > read_so_far)) {
			mm_free(mem);
			errno = e;
			goto err;
		} else if (pos < 0) {
			mm_free(mem);
			goto err;
		}

		seg->contents = mem;
	}

done:
	return 0;
err:
	return -1;
}