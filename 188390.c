xps_parse_glyphs_imp(xps_document *doc, const fz_matrix *ctm,
	fz_font *font, float size, float originx, float originy,
	int is_sideways, int bidi_level,
	char *indices, char *unicode)
{
	xps_glyph_metrics mtx;
	fz_text *text;
	fz_matrix tm;
	float e, f;
	float x = originx;
	float y = originy;
	char *us = unicode;
	char *is = indices;
	int un = 0;

	if (!unicode && !indices)
		fz_warn(doc->ctx, "glyphs element with neither characters nor indices");

	if (us)
	{
		if (us[0] == '{' && us[1] == '}')
			us = us + 2;
		un = strlen(us);
	}

	if (is_sideways)
	{
		fz_pre_scale(fz_rotate(&tm, 90), -size, size);
	}
	else
		fz_scale(&tm, size, -size);

	text = fz_new_text(doc->ctx, font, &tm, is_sideways);

	while ((us && un > 0) || (is && *is))
	{
		int char_code = '?';
		int code_count = 1;
		int glyph_count = 1;

		if (is && *is)
		{
			is = xps_parse_cluster_mapping(is, &code_count, &glyph_count);
		}

		if (code_count < 1)
			code_count = 1;
		if (glyph_count < 1)
			glyph_count = 1;

		/* TODO: add code chars with cluster mappings for text extraction */

		while (code_count--)
		{
			if (us && un > 0)
			{
				int t = fz_chartorune(&char_code, us);
				us += t; un -= t;
			}
		}

		while (glyph_count--)
		{
			int glyph_index = -1;
			float u_offset = 0;
			float v_offset = 0;
			float advance;

			if (is && *is)
				is = xps_parse_glyph_index(is, &glyph_index);

			if (glyph_index == -1)
				glyph_index = xps_encode_font_char(font, char_code);

			xps_measure_font_glyph(doc, font, glyph_index, &mtx);
			if (is_sideways)
				advance = mtx.vadv * 100;
			else if (bidi_level & 1)
				advance = -mtx.hadv * 100;
			else
				advance = mtx.hadv * 100;

			if (font->ft_bold)
				advance *= 1.02f;

			if (is && *is)
			{
				is = xps_parse_glyph_metrics(is, &advance, &u_offset, &v_offset);
				if (*is == ';')
					is ++;
			}

			if (bidi_level & 1)
				u_offset = -mtx.hadv * 100 - u_offset;

			u_offset = u_offset * 0.01f * size;
			v_offset = v_offset * 0.01f * size;

			if (is_sideways)
			{
				e = x + u_offset + (mtx.vorg * size);
				f = y - v_offset + (mtx.hadv * 0.5f * size);
			}
			else
			{
				e = x + u_offset;
				f = y - v_offset;
			}

			fz_add_text(doc->ctx, text, glyph_index, char_code, e, f);

			x += advance * 0.01f * size;
		}
	}

	return text;
}