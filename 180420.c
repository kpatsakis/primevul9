new_msg_del_if (u_int32_t seqnr, struct in_addr ifaddr)
{
  struct msg_del_if dmsg;

  dmsg.ifaddr = ifaddr;

  return msg_new (MSG_DEL_IF, &dmsg, seqnr, sizeof (struct msg_del_if));
}