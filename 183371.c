ptp_unpack_EOS_FocusInfoEx (PTPParams* params, unsigned char** data, uint32_t datasize )
{
	uint32_t size 			= dtoh32a( *data );
	uint32_t halfsize		= dtoh16a( (*data) + 4);
	uint32_t version		= dtoh16a( (*data) + 6);
	uint32_t focus_points_in_struct	= dtoh16a( (*data) + 8);
	uint32_t focus_points_in_use	= dtoh16a( (*data) + 10);
	uint32_t sizeX			= dtoh16a( (*data) + 12);
	uint32_t sizeY			= dtoh16a( (*data) + 14);
	uint32_t size2X			= dtoh16a( (*data) + 16);
	uint32_t size2Y			= dtoh16a( (*data) + 18);
	uint32_t i;
	uint32_t maxlen;
	char	*str, *p;

	if ((size >= datasize) || (size < 20))
		return strdup("bad size 1");
	/* every focuspoint gets 4 (16 bit number possible "-" sign and a x) and a ,*/
	/* inital things around lets say 100 chars at most. 
	 * FIXME: check selected when we decode it
	 */
	if (size < focus_points_in_struct*8) {
		ptp_error(params, "focus_points_in_struct %d is too large vs size %d", focus_points_in_struct, size);
		return strdup("bad size 2");
	}
	if (focus_points_in_use > focus_points_in_struct) {
		ptp_error(params, "focus_points_in_use %d is larger than focus_points_in_struct %d", focus_points_in_use, focus_points_in_struct);
		return strdup("bad size 3");
	}

	maxlen = focus_points_in_use*32 + 100 + (size - focus_points_in_struct*8)*2;
	if (halfsize != size-4) {
		ptp_error(params, "halfsize %d is not expected %d", halfsize, size-4);
		return strdup("bad size 4");
	}
	if (20 + focus_points_in_struct*8 + (focus_points_in_struct+7)/8 > size) {
		ptp_error(params, "size %d is too large for fp in struct %d", focus_points_in_struct*8 + 20 + (focus_points_in_struct+7)/8, size);
		return strdup("bad size 5");
	}
#if 0
	ptp_debug(params,"d1d3 content:");
	for (i=0;i<size;i+=2)
		ptp_debug(params,"%d: %02x %02x", i, (*data)[i], (*data)[i+1]);
#endif
	ptp_debug(params,"d1d3 version %d", version);
	ptp_debug(params,"d1d3 size %d", size);
	ptp_debug(params,"d1d3 focus points in struct %d, in use %d", focus_points_in_struct, focus_points_in_use);

	str = (char*)malloc( maxlen );
	if (!str)
		return NULL;
	p = str;

	p += sprintf(p,"eosversion=%d,size=%dx%d,size2=%dx%d,points={", version, sizeX, sizeY, size2X, size2Y);
	for (i=0;i<focus_points_in_use;i++) {
		int16_t x = dtoh16a((*data) + focus_points_in_struct*4 + 20 + 2*i);
		int16_t y = dtoh16a((*data) + focus_points_in_struct*6 + 20 + 2*i);
		int16_t w = dtoh16a((*data) + focus_points_in_struct*2 + 20 + 2*i);
		int16_t h = dtoh16a((*data) + focus_points_in_struct*0 + 20 + 2*i);

		p += sprintf(p,"{%d,%d,%d,%d}",x,y,w,h);

		if (i<focus_points_in_use-1)
			p += sprintf(p,",");
	}
	p += sprintf(p,"},select={");
	for (i=0;i<focus_points_in_use;i++) {
		if ((1<<(i%8)) & ((*data)[focus_points_in_struct*8+20+i/8]))
			p+=sprintf(p,"%d,", i);
	}

	p += sprintf(p,"},unknown={");
	for (i=focus_points_in_struct*8+(focus_points_in_struct+7)/8+20;i<size;i++) {
		if ((p-str) > maxlen - 4)
			break;
		p+=sprintf(p,"%02x", (*data)[i]);
	}
	p += sprintf(p,"}");
	return str;
}