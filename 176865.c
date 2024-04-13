gst_matroska_track_encoding_scope_name (gint val)
{
  GEnumValue *en;
  GEnumClass *enum_class =
      g_type_class_ref (MATROSKA_TRACK_ENCODING_SCOPE_TYPE);

  en = g_enum_get_value (G_ENUM_CLASS (enum_class), val);
  return en ? en->value_nick : NULL;
}