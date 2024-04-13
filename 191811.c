dirserv_read_guardfraction_file(const char *fname,
                             smartlist_t *vote_routerstatuses)
{
  char *guardfraction_file_str;

  /* Read file to a string */
  guardfraction_file_str = read_file_to_str(fname, RFTS_IGNORE_MISSING, NULL);
  if (!guardfraction_file_str) {
      log_warn(LD_FS, "Cannot open guardfraction file '%s'. Failing.", fname);
      return -1;
  }

  return dirserv_read_guardfraction_file_from_str(guardfraction_file_str,
                                               vote_routerstatuses);
}