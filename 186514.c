png_handle_eXIf(png_structrp png_ptr, png_inforp info_ptr, png_uint_32 length)
{
   unsigned int i;

   png_debug(1, "in png_handle_eXIf");

   if ((png_ptr->mode & PNG_HAVE_IHDR) == 0)
      png_chunk_error(png_ptr, "missing IHDR");

   else if (info_ptr == NULL || (info_ptr->valid & PNG_INFO_eXIf) != 0)
   {
      png_crc_finish(png_ptr, length);
      png_chunk_benign_error(png_ptr, "duplicate");
      return;
   }

   info_ptr->eXIf_buf = png_voidcast(png_bytep,
             png_malloc_warn(png_ptr, length));

   if (info_ptr->eXIf_buf == NULL)
   {
      png_crc_finish(png_ptr, length);
      png_chunk_benign_error(png_ptr, "out of memory");
      return;
   }

   info_ptr->free_me |= PNG_FREE_EXIF;
   for (i = 0; i < length; i++)
   {
      png_byte buf[1];
      png_crc_read(png_ptr, buf, 1);
      info_ptr->eXIf_buf[i] = buf[0];
   }

   if (png_crc_finish(png_ptr, 0) != 0)
      return;

   png_set_eXIf_1(png_ptr, info_ptr, length, info_ptr->eXIf_buf);

   png_free(png_ptr, info_ptr->eXIf_buf);
   info_ptr->eXIf_buf = NULL;
}