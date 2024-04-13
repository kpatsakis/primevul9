static int getthebyte(jas_stream_t *in, long off)
{
	int c;
	long oldpos;
	oldpos = jas_stream_tell(in);
	assert(oldpos >= 0);
	jas_stream_seek(in, off, SEEK_SET);
	c = jas_stream_peekc(in);
	jas_stream_seek(in, oldpos, SEEK_SET);
	return c;
}