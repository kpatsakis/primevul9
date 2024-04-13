ptp_unpack_EOS_ImageFormat (PTPParams* params, unsigned char** data )
{
	/*
	  EOS ImageFormat entries (of at least the 5DM2 and the 400D) look like this:
		uint32: number of entries / generated files (1 or 2)
		uint32: size of this entry in bytes (most likely allways 0x10)
		uint32: image type (1 == JPG, 6 == RAW)
		uint32: image size (0 == Large, 1 == Medium, 2 == Small, 0xe == S1, 0xf == S2, 0x10 == S3)
		uint32: image compression (2 == Standard/JPG, 3 == Fine/JPG, 4 == Lossles/RAW)
	  If the number of entries is 2 the last 4 uint32 repeat.

	  example:
		0: 0x       1
		1: 0x      10
		2: 0x       6
		3: 0x       1
		4: 0x       4

	  The idea is to simply 'condense' these values to just one uint16 to be able to conveniently
	  use the available enumeration facilities (look-up table). The image size and compression
	  values fully describe the image format. Hence we generate a uint16 with the four nibles set
	  as follows: entry 1 size | entry 1 compression | entry 2 size | entry 2 compression.
	  The above example would result in the value 0x1400.

	  The EOS 5D Mark III (and possibly other high-end EOS as well) added the extra fancy S1, S2
	  and S3 JPEG options. S1 replaces the old Small. -1 the S1/S2/S3 to prevent the 0x10 overflow.
	  */

	const unsigned char* d = *data;
	uint32_t n = dtoh32a( d );
	uint32_t l, s1, c1, s2 = 0, c2 = 0;

	if (n != 1 && n !=2) {
		ptp_debug (params, "parsing EOS ImageFormat property failed (n != 1 && n != 2: %d)", n);
		return 0;
	}

	l = dtoh32a( d+=4 );
	if (l != 0x10) {
		ptp_debug (params, "parsing EOS ImageFormat property failed (l != 0x10: 0x%x)", l);
		return 0;
	}

	d+=4; /* skip type */
	s1 = dtoh32a( d+=4 );
	c1 = dtoh32a( d+=4 );

	if (n == 2) {
		l = dtoh32a( d+=4 );
		if (l != 0x10) {
			ptp_debug (params, "parsing EOS ImageFormat property failed (l != 0x10: 0x%x)", l);
			return 0;
		}
		d+=4; /* skip type */
		s2 = dtoh32a( d+=4 );
		c2 = dtoh32a( d+=4 );
	}

	*data = (unsigned char*) d+4;

	/* deal with S1/S2/S3 JPEG sizes, see above. */
	if( s1 >= 0xe )
		s1--;
	if( s2 >= 0xe )
		s2--;

	return ((s1 & 0xF) << 12) | ((c1 & 0xF) << 8) | ((s2 & 0xF) << 4) | ((c2 & 0xF) << 0);
}