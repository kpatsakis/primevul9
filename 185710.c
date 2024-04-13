hb_ot_get_glyph_extents (hb_font_t *font HB_UNUSED,
			 void *font_data,
			 hb_codepoint_t glyph,
			 hb_glyph_extents_t *extents,
			 void *user_data HB_UNUSED)
{
  const hb_ot_font_t *ot_font = (const hb_ot_font_t *) font_data;
  bool ret = ot_font->glyf.get_extents (glyph, extents);
  extents->x_bearing = font->em_scale_x (extents->x_bearing);
  extents->y_bearing = font->em_scale_y (extents->y_bearing);
  extents->width     = font->em_scale_x (extents->width);
  extents->height    = font->em_scale_y (extents->height);
  return ret;
}