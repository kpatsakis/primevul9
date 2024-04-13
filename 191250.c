qtdemux_uuid_bytes_to_string (gconstpointer uuid_bytes)
{
  const guint8 *uuid = (const guint8 *) uuid_bytes;

  return g_strdup_printf ("%02x%02x%02x%02x-%02x%02x-%02x%02x-"
      "%02x%02x-%02x%02x%02x%02x%02x%02x",
      uuid[0], uuid[1], uuid[2], uuid[3],
      uuid[4], uuid[5], uuid[6], uuid[7],
      uuid[8], uuid[9], uuid[10], uuid[11],
      uuid[12], uuid[13], uuid[14], uuid[15]);
}