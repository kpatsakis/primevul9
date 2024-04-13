ptp_unpack_uint32_t_array(PTPParams *params, unsigned char* data, unsigned int offset, unsigned int datalen, uint32_t **array)
{
	uint32_t n, i=0;

	if (!data)
		return 0;

	if (offset >= datalen)
		return 0;

	if (offset + sizeof(uint32_t) > datalen)
		return 0;

	*array = NULL;
	n=dtoh32a(&data[offset]);
	if (n >= UINT_MAX/sizeof(uint32_t))
		return 0;
	if (!n)
		return 0;

	if (offset + sizeof(uint32_t)*(n+1) > datalen) {
		ptp_debug (params ,"array runs over datalen bufferend (%d vs %d)", offset + sizeof(uint32_t)*(n+1) , datalen);
		return 0;
	}

	*array = malloc (n*sizeof(uint32_t));
	if (!*array)
		return 0;
	for (i=0;i<n;i++)
		(*array)[i]=dtoh32a(&data[offset+(sizeof(uint32_t)*(i+1))]);
	return n;
}