void evbuffer_file_segment_add_cleanup_cb(struct evbuffer_file_segment *seg,
	evbuffer_file_segment_cleanup_cb cb, void* arg)
{
	EVUTIL_ASSERT(seg->refcnt > 0);
	seg->cleanup_cb = cb;
	seg->cleanup_cb_arg = arg;
}