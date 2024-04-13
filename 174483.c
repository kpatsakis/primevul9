process_fontcache(STREAM s)
{
	RD_HGLYPH bitmap;
	uint8 font, nglyphs;
	uint16 character, offset, baseline, width, height;
	int i, datasize;
	uint8 *data;

	in_uint8(s, font);
	in_uint8(s, nglyphs);

	DEBUG(("FONTCACHE(font=%d,n=%d)\n", font, nglyphs));

	for (i = 0; i < nglyphs; i++)
	{
		in_uint16_le(s, character);
		in_uint16_le(s, offset);
		in_uint16_le(s, baseline);
		in_uint16_le(s, width);
		in_uint16_le(s, height);

		datasize = (height * ((width + 7) / 8) + 3) & ~3;
		in_uint8p(s, data, datasize);

		bitmap = ui_create_glyph(width, height, data);
		cache_put_font(font, character, offset, baseline, width, height, bitmap);
	}
}