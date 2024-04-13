read_image_resource_block (PSDimage  *img_a,
                           FILE      *f,
                           GError   **error)
{
  guint32 block_len;
  guint32 block_end;

  if (fread (&block_len, 4, 1, f) < 1)
    {
      psd_set_error (feof (f), errno, error);
      return -1;
    }
  img_a->image_res_len = GUINT32_FROM_BE (block_len);

  IFDBG(1) g_debug ("Image resource block size = %d", (int)img_a->image_res_len);

  img_a->image_res_start = ftell (f);
  block_end = img_a->image_res_start + img_a->image_res_len;

  if (fseek (f, block_end, SEEK_SET) < 0)
    {
      psd_set_error (feof (f), errno, error);
      return -1;
    }

  return 0;
}