send_newstyle_option_reply_info_export (uint32_t option, uint32_t reply,
                                        uint16_t info, uint64_t exportsize)
{
  GET_CONN;
  struct nbd_fixed_new_option_reply fixed_new_option_reply;
  struct nbd_fixed_new_option_reply_info_export export;

  fixed_new_option_reply.magic = htobe64 (NBD_REP_MAGIC);
  fixed_new_option_reply.option = htobe32 (option);
  fixed_new_option_reply.reply = htobe32 (reply);
  fixed_new_option_reply.replylen = htobe32 (sizeof export);
  export.info = htobe16 (info);
  export.exportsize = htobe64 (exportsize);
  export.eflags = htobe16 (conn->eflags);

  if (conn->send (&fixed_new_option_reply,
                  sizeof fixed_new_option_reply, SEND_MORE) == -1 ||
      conn->send (&export, sizeof export, 0) == -1) {
    nbdkit_error ("write: %s: %m", name_of_nbd_opt (option));
    return -1;
  }

  return 0;
}