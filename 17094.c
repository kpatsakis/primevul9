send_newstyle_option_reply (uint32_t option, uint32_t reply)
{
  GET_CONN;
  struct nbd_fixed_new_option_reply fixed_new_option_reply;

  fixed_new_option_reply.magic = htobe64 (NBD_REP_MAGIC);
  fixed_new_option_reply.option = htobe32 (option);
  fixed_new_option_reply.reply = htobe32 (reply);
  fixed_new_option_reply.replylen = htobe32 (0);

  if (conn->send (&fixed_new_option_reply,
                  sizeof fixed_new_option_reply, 0) == -1) {
    /* The protocol document says that the client is allowed to simply
     * drop the connection after sending NBD_OPT_ABORT, or may read
     * the reply.
     */
    if (option == NBD_OPT_ABORT)
      debug ("write: %s: %m", name_of_nbd_opt (option));
    else
      nbdkit_error ("write: %s: %m", name_of_nbd_opt (option));
    return -1;
  }

  return 0;
}