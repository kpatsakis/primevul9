_tiff_seek (thandle_t handle,
	    toff_t    offset,
	    int       whence)
{
	TiffBuffer *buffer = (TiffBuffer *)handle;

	switch (whence) {
	case SEEK_SET:
		if (offset > buffer->buffer_len)
			return -1;
		buffer->pos = offset;
		break;
	case SEEK_CUR:
		if (offset + buffer->pos >= buffer->buffer_len)
			return -1;
		buffer->pos += offset;
		break;
	case SEEK_END:
		if (offset + buffer->buffer_len > buffer->buffer_len)
			return -1;
		buffer->pos = buffer->buffer_len + offset;
		break;
	default:
		return -1;
	}

	return buffer->pos;
}