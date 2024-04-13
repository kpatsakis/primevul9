_tiff_read (thandle_t handle,
	    tdata_t   buf,
	    tsize_t   size)
{
	TiffBuffer *buffer = (TiffBuffer *)handle;

	if (buffer->pos + size > buffer->buffer_len)
		return 0;

	memcpy (buf, buffer->buffer + buffer->pos, size);
	buffer->pos += size;

	return size;
}