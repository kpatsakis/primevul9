gst_matroska_track_encryption_cipher_mode_name (gint val)
{
  GEnumValue *en;
  GEnumClass *enum_class =
      g_type_class_ref (MATROSKA_TRACK_ENCRYPTION_CIPHER_MODE_TYPE);
  en = g_enum_get_value (G_ENUM_CLASS (enum_class), val);
  return en ? en->value_nick : NULL;
}