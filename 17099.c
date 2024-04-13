send_newstyle_option_reply_meta_context (uint32_t option, uint32_t reply,
                                         uint32_t context_id,
                                         const char *name)
{
  GET_CONN;
  struct nbd_fixed_new_option_reply fixed_new_option_reply;
  struct nbd_fixed_new_option_reply_meta_context context;
  const size_t namelen = strlen (name);

  debug ("newstyle negotiation: %s: replying with %s id %d",
         name_of_nbd_opt (option), name, context_id);
  fixed_new_option_reply.magic = htobe64 (NBD_REP_MAGIC);
  fixed_new_option_reply.option = htobe32 (option);
  fixed_new_option_reply.reply = htobe32 (reply);
  fixed_new_option_reply.replylen = htobe32 (sizeof context + namelen);
  context.context_id = htobe32 (context_id);

  if (conn->send (&fixed_new_option_reply,
                  sizeof fixed_new_option_reply, SEND_MORE) == -1 ||
      conn->send (&context, sizeof context, SEND_MORE) == -1 ||
      conn->send (name, namelen, 0) == -1) {
    nbdkit_error ("write: %s: %m", name_of_nbd_opt (option));
    return -1;
  }

  return 0;
}