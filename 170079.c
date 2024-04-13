static gboolean grow_buffer (struct bmp_progressive_state *State,
                             GError **error)
{
  guchar *tmp;

  if (State->BufferSize == 0) {
    g_set_error_literal (error,
                         GDK_PIXBUF_ERROR,
                         GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                         _("BMP image has bogus header data"));
    State->read_state = READ_STATE_ERROR;
    return FALSE;
  }

  tmp = g_try_realloc (State->buff, State->BufferSize);

  if (!tmp) {
    g_set_error_literal (error,
                         GDK_PIXBUF_ERROR,
                         GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
                         _("Not enough memory to load bitmap image"));
    State->read_state = READ_STATE_ERROR;
    return FALSE;
  }

  State->buff = tmp;
  return TRUE;
}