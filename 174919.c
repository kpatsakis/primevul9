free_codes(struct archive_read *a)
{
  struct rar *rar = (struct rar *)(a->format->data);
  free(rar->maincode.tree);
  free(rar->offsetcode.tree);
  free(rar->lowoffsetcode.tree);
  free(rar->lengthcode.tree);
  free(rar->maincode.table);
  free(rar->offsetcode.table);
  free(rar->lowoffsetcode.table);
  free(rar->lengthcode.table);
  memset(&rar->maincode, 0, sizeof(rar->maincode));
  memset(&rar->offsetcode, 0, sizeof(rar->offsetcode));
  memset(&rar->lowoffsetcode, 0, sizeof(rar->lowoffsetcode));
  memset(&rar->lengthcode, 0, sizeof(rar->lengthcode));
}