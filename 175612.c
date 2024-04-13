evbuffer_file_segment_free(struct evbuffer_file_segment *seg)
{
	int refcnt;
	EVLOCK_LOCK(seg->lock, 0);
	refcnt = --seg->refcnt;
	EVLOCK_UNLOCK(seg->lock, 0);
	if (refcnt > 0)
		return;
	EVUTIL_ASSERT(refcnt == 0);

	if (seg->is_mapping) {
#ifdef _WIN32
		CloseHandle(seg->mapping_handle);
#elif defined (EVENT__HAVE_MMAP)
		off_t offset_leftover;
		offset_leftover = seg->file_offset % get_page_size();
		if (munmap(seg->mapping, seg->length + offset_leftover) == -1)
			event_warn("%s: munmap failed", __func__);
#endif
	} else if (seg->contents) {
		mm_free(seg->contents);
	}

	if ((seg->flags & EVBUF_FS_CLOSE_ON_FREE) && seg->fd >= 0) {
		close(seg->fd);
	}
	
	if (seg->cleanup_cb) {
		(*seg->cleanup_cb)((struct evbuffer_file_segment const*)seg, 
		    seg->flags, seg->cleanup_cb_arg);
		seg->cleanup_cb = NULL;
		seg->cleanup_cb_arg = NULL;
	}

	EVTHREAD_FREE_LOCK(seg->lock, 0);
	mm_free(seg);
}