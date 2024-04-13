_hb_ot_font_destroy (hb_ot_font_t *ot_font)
{
  ot_font->cmap.fini ();
  ot_font->h_metrics.fini ();
  ot_font->v_metrics.fini ();
  ot_font->glyf.fini ();

  free (ot_font);
}