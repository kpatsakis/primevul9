xmlBufferShrink(xmlBufferPtr buf, unsigned int len) {
    if (buf == NULL) return(-1);
    if (len == 0) return(0);
    if (len > buf->use) return(-1);

    buf->use -= len;
    if ((buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) ||
        ((buf->alloc == XML_BUFFER_ALLOC_IO) && (buf->contentIO != NULL))) {
	/*
	 * we just move the content pointer, but also make sure
	 * the perceived buffer size has shrinked accordingly
	 */
        buf->content += len;
	buf->size -= len;

        /*
	 * sometimes though it maybe be better to really shrink
	 * on IO buffers
	 */
	if ((buf->alloc == XML_BUFFER_ALLOC_IO) && (buf->contentIO != NULL)) {
	    size_t start_buf = buf->content - buf->contentIO;
	    if (start_buf >= buf->size) {
		memmove(buf->contentIO, &buf->content[0], buf->use);
		buf->content = buf->contentIO;
		buf->content[buf->use] = 0;
		buf->size += start_buf;
	    }
	}
    } else {
	memmove(buf->content, &buf->content[len], buf->use);
	buf->content[buf->use] = 0;
    }
    return(len);
}