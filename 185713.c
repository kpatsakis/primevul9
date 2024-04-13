_hb_ot_font_create (hb_face_t *face)
{
  hb_ot_font_t *ot_font = (hb_ot_font_t *) calloc (1, sizeof (hb_ot_font_t));

  if (unlikely (!ot_font))
    return NULL;

  unsigned int upem = face->get_upem ();

  ot_font->cmap.init (face);
  ot_font->h_metrics.init (face, HB_OT_TAG_hhea, HB_OT_TAG_hmtx, upem>>1);
  ot_font->v_metrics.init (face, HB_OT_TAG_vhea, HB_OT_TAG_vmtx, upem); /* TODO Can we do this lazily? */
  ot_font->glyf.init (face);

  return ot_font;
}