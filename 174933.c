archive_read_format_rar_cleanup(struct archive_read *a)
{
  struct rar *rar;

  rar = (struct rar *)(a->format->data);
  free_codes(a);
  free(rar->filename);
  free(rar->filename_save);
  free(rar->dbo);
  free(rar->unp_buffer);
  free(rar->lzss.window);
  __archive_ppmd7_functions.Ppmd7_Free(&rar->ppmd7_context);
  free(rar);
  (a->format->data) = NULL;
  return (ARCHIVE_OK);
}