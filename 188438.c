xps_measure_font_glyph(xps_document *doc, fz_font *font, int gid, xps_glyph_metrics *mtx)
{
	int mask = FT_LOAD_NO_SCALE | FT_LOAD_IGNORE_TRANSFORM;
	FT_Face face = font->ft_face;
	FT_Fixed hadv, vadv;
	fz_context *ctx = doc->ctx;

	fz_lock(ctx, FZ_LOCK_FREETYPE);
	FT_Get_Advance(face, gid, mask, &hadv);
	FT_Get_Advance(face, gid, mask | FT_LOAD_VERTICAL_LAYOUT, &vadv);
	fz_unlock(ctx, FZ_LOCK_FREETYPE);

	mtx->hadv = hadv / (float)face->units_per_EM;
	mtx->vadv = vadv / (float)face->units_per_EM;
	mtx->vorg = face->ascender / (float) face->units_per_EM;
}