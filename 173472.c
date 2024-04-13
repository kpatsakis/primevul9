read_merged_image_block (PSDimage  *img_a,
                         FILE      *f,
                         GError   **error)
{
  img_a->merged_image_start = ftell(f);
  if (fseek (f, 0, SEEK_END) < 0)
    {
      psd_set_error (feof (f), errno, error);
      return -1;
    }

  img_a->merged_image_len = ftell(f) - img_a->merged_image_start;

  IFDBG(1) g_debug ("Merged image data block: Start: %d, len: %d",
                     img_a->merged_image_start, img_a->merged_image_len);

  return 0;
}