finish_newstyle_options (uint64_t *exportsize,
                         const char *exportname_in, uint32_t exportnamelen)
{
  GET_CONN;

  /* Since the exportname string passed here comes directly out of the
   * NBD protocol make a temporary copy of the exportname into a
   * \0-terminated buffer.
   */
  CLEANUP_FREE char *exportname = strndup (exportname_in, exportnamelen);
  if (exportname == NULL) {
    nbdkit_error ("strndup: %m");
    return -1;
  }

  /* The NBD spec says that if the client later uses NBD_OPT_GO on a
   * different export, then the context from the earlier
   * NBD_OPT_SET_META_CONTEXT is not usable so discard it.
   */
  if (conn->exportname_from_set_meta_context &&
      strcmp (conn->exportname_from_set_meta_context, exportname) != 0) {
    debug ("newstyle negotiation: NBD_OPT_SET_META_CONTEXT export name \"%s\" "
           "≠ final client exportname \"%s\", "
           "so discarding the previous context",
           conn->exportname_from_set_meta_context, exportname);
    conn->meta_context_base_allocation = false;
  }

  if (protocol_common_open (exportsize, &conn->eflags, exportname) == -1)
    return -1;

  debug ("newstyle negotiation: flags: export 0x%x", conn->eflags);
  return 0;
}