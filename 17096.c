send_newstyle_option_reply_info_str (uint32_t option, uint32_t reply,
                                     uint16_t info, const char *str,
                                     size_t len)
{
  GET_CONN;
  struct nbd_fixed_new_option_reply fixed_new_option_reply;
  struct nbd_fixed_new_option_reply_info_name_or_desc name;

  if (len == -1)
    len = strlen (str);
  assert (len <= NBD_MAX_STRING);

  fixed_new_option_reply.magic = htobe64 (NBD_REP_MAGIC);
  fixed_new_option_reply.option = htobe32 (option);
  fixed_new_option_reply.reply = htobe32 (reply);
  fixed_new_option_reply.replylen = htobe32 (sizeof info + len);
  name.info = htobe16 (info);

  if (conn->send (&fixed_new_option_reply,
                  sizeof fixed_new_option_reply, SEND_MORE) == -1 ||
      conn->send (&name, sizeof name, SEND_MORE) == -1 ||
      conn->send (str, len, 0) == -1) {
    nbdkit_error ("write: %s: %m", name_of_nbd_opt (option));
    return -1;
  }

  return 0;
}