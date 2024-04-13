ptp_pack_EOS_ImageFormat (PTPParams* params, unsigned char* data, uint16_t value)
{
	uint32_t n = (value & 0xFF) ? 2 : 1;
	uint32_t s = 4 + 0x10 * n;

	if( !data )
		return s;

#define PACK_5DM3_SMALL_JPEG_SIZE( X ) (X) >= 0xd ? (X)+1 : (X)

	htod32a(data+=0, n);
	htod32a(data+=4, 0x10);
	htod32a(data+=4, ((value >> 8) & 0xF) == 4 ? 6 : 1);
	htod32a(data+=4, PACK_5DM3_SMALL_JPEG_SIZE((value >> 12) & 0xF));
	htod32a(data+=4, (value >> 8) & 0xF);

	if (n==2) {
		htod32a(data+=4, 0x10);
		htod32a(data+=4, ((value >> 0) & 0xF) == 4 ? 6 : 1);
		htod32a(data+=4, PACK_5DM3_SMALL_JPEG_SIZE((value >> 4) & 0xF));
		htod32a(data+=4, (value >> 0) & 0xF);
	}

#undef PACK_5DM3_SMALL_JPEG_SIZE

	return s;
}