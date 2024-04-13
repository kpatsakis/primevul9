_gst_buffer_new_wrapped (gpointer mem, gsize size, GFreeFunc free_func)
{
  return gst_buffer_new_wrapped_full (free_func ? 0 : GST_MEMORY_FLAG_READONLY,
      mem, size, 0, size, mem, free_func);
}