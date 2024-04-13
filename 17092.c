send_newstyle_option_reply_exportnames (uint32_t option, size_t *nr_options)
{
  GET_CONN;
  struct nbd_fixed_new_option_reply fixed_new_option_reply;
  size_t i, list_len;
  CLEANUP_EXPORTS_FREE struct nbdkit_exports *exps = NULL;

  exps = nbdkit_exports_new ();
  if (exps == NULL)
    return send_newstyle_option_reply (option, NBD_REP_ERR_TOO_BIG);
  if (backend_list_exports (top, read_only, exps) == -1)
    return send_newstyle_option_reply (option, NBD_REP_ERR_PLATFORM);

  /* Allow additional per-export NBD_OPT_INFO and friends. */
  list_len = nbdkit_exports_count (exps);
  *nr_options += MAX_NR_OPTIONS * list_len;

  for (i = 0; i < list_len; i++) {
    const struct nbdkit_export export = nbdkit_get_export (exps, i);
    size_t name_len = strlen (export.name);
    size_t desc_len = export.description ? strlen (export.description) : 0;
    uint32_t len;

    fixed_new_option_reply.magic = htobe64 (NBD_REP_MAGIC);
    fixed_new_option_reply.option = htobe32 (option);
    fixed_new_option_reply.reply = htobe32 (NBD_REP_SERVER);
    fixed_new_option_reply.replylen = htobe32 (name_len + sizeof (len) +
                                               desc_len);

    if (conn->send (&fixed_new_option_reply,
                    sizeof fixed_new_option_reply, SEND_MORE) == -1) {
      nbdkit_error ("write: %s: %m", name_of_nbd_opt (option));
      return -1;
    }

    len = htobe32 (name_len);
    if (conn->send (&len, sizeof len, SEND_MORE) == -1) {
      nbdkit_error ("write: %s: %s: %m",
                    name_of_nbd_opt (option), "sending length");
      return -1;
    }
    if (conn->send (export.name, name_len, SEND_MORE) == -1) {
      nbdkit_error ("write: %s: %s: %m",
                    name_of_nbd_opt (option), "sending export name");
      return -1;
    }
    if (conn->send (export.description, desc_len, 0) == -1) {
      nbdkit_error ("write: %s: %s: %m",
                    name_of_nbd_opt (option), "sending export description");
      return -1;
    }
  }

  return send_newstyle_option_reply (option, NBD_REP_ACK);
}