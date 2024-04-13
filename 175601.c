evbuffer_add_file_segment(struct evbuffer *buf,
    struct evbuffer_file_segment *seg, ev_off_t offset, ev_off_t length)
{
	struct evbuffer_chain *chain;
	struct evbuffer_chain_file_segment *extra;
	int can_use_sendfile = 0;

	EVBUFFER_LOCK(buf);
	EVLOCK_LOCK(seg->lock, 0);
	if (buf->flags & EVBUFFER_FLAG_DRAINS_TO_FD) {
		can_use_sendfile = 1;
	} else {
		if (!seg->contents) {
			if (evbuffer_file_segment_materialize(seg)<0) {
				EVLOCK_UNLOCK(seg->lock, 0);
				EVBUFFER_UNLOCK(buf);
				return -1;
			}
		}
	}
	++seg->refcnt;
	EVLOCK_UNLOCK(seg->lock, 0);

	if (buf->freeze_end)
		goto err;

	if (length < 0) {
		if (offset > seg->length)
			goto err;
		length = seg->length - offset;
	}

	/* Can we actually add this? */
	if (offset+length > seg->length)
		goto err;

	chain = evbuffer_chain_new(sizeof(struct evbuffer_chain_file_segment));
	if (!chain)
		goto err;
	extra = EVBUFFER_CHAIN_EXTRA(struct evbuffer_chain_file_segment, chain);

	chain->flags |= EVBUFFER_IMMUTABLE|EVBUFFER_FILESEGMENT;
	if (can_use_sendfile && seg->can_sendfile) {
		chain->flags |= EVBUFFER_SENDFILE;
		chain->misalign = seg->file_offset + offset;
		chain->off = length;
		chain->buffer_len = chain->misalign + length;
	} else if (seg->is_mapping) {
#ifdef _WIN32
		ev_uint64_t total_offset = seg->mmap_offset+offset;
		ev_uint64_t offset_rounded=0, offset_remaining=0;
		LPVOID data;
		if (total_offset) {
			SYSTEM_INFO si;
			memset(&si, 0, sizeof(si)); /* cargo cult */
			GetSystemInfo(&si);
			offset_remaining = total_offset % si.dwAllocationGranularity;
			offset_rounded = total_offset - offset_remaining;
		}
		data = MapViewOfFile(
			seg->mapping_handle,
			FILE_MAP_READ,
			offset_rounded >> 32,
			offset_rounded & 0xfffffffful,
			length + offset_remaining);
		if (data == NULL) {
			mm_free(chain);
			goto err;
		}
		chain->buffer = (unsigned char*) data;
		chain->buffer_len = length+offset_remaining;
		chain->misalign = offset_remaining;
		chain->off = length;
#else
		chain->buffer = (unsigned char*)(seg->contents + offset);
		chain->buffer_len = length;
		chain->off = length;
#endif
	} else {
		chain->buffer = (unsigned char*)(seg->contents + offset);
		chain->buffer_len = length;
		chain->off = length;
	}

	extra->segment = seg;
	buf->n_add_for_cb += length;
	evbuffer_chain_insert(buf, chain);

	evbuffer_invoke_callbacks_(buf);

	EVBUFFER_UNLOCK(buf);

	return 0;
err:
	EVBUFFER_UNLOCK(buf);
	evbuffer_file_segment_free(seg); /* Lowers the refcount */
	return -1;
}