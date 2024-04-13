evbuffer_add_file(struct evbuffer *buf, int fd, ev_off_t offset, ev_off_t length)
{
	struct evbuffer_file_segment *seg;
	unsigned flags = EVBUF_FS_CLOSE_ON_FREE;
	int r;

	seg = evbuffer_file_segment_new(fd, offset, length, flags);
	if (!seg)
		return -1;
	r = evbuffer_add_file_segment(buf, seg, 0, length);
	if (r == 0)
		evbuffer_file_segment_free(seg);
	return r;
}