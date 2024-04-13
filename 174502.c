setup_brush(BRUSH * out_brush, BRUSH * in_brush)
{
	BRUSHDATA *brush_data;
	uint8 cache_idx;
	uint8 colour_code;

	memcpy(out_brush, in_brush, sizeof(BRUSH));
	if (out_brush->style & 0x80)
	{
		colour_code = out_brush->style & 0x0f;
		cache_idx = out_brush->pattern[0];
		brush_data = cache_get_brush_data(colour_code, cache_idx);
		if ((brush_data == NULL) || (brush_data->data == NULL))
		{
			error("error getting brush data, style %x\n", out_brush->style);
			out_brush->bd = NULL;
			memset(out_brush->pattern, 0, 8);
		}
		else
		{
			out_brush->bd = brush_data;
		}
		out_brush->style = 3;
	}
}