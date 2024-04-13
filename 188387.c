pango_unichar_direction (gunichar ch)
{
  FriBidiCharType fribidi_ch_type;

  G_STATIC_ASSERT (sizeof (FriBidiChar) == sizeof (gunichar));

  fribidi_ch_type = fribidi_get_bidi_type (ch);

  if (!FRIBIDI_IS_STRONG (fribidi_ch_type))
    return PANGO_DIRECTION_NEUTRAL;
  else if (FRIBIDI_IS_RTL (fribidi_ch_type))
    return PANGO_DIRECTION_RTL;
  else
    return PANGO_DIRECTION_LTR;
}