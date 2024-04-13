new_msg_new_if (u_int32_t seqnr,
		struct in_addr ifaddr, struct in_addr area_id)
{
  struct msg_new_if nmsg;

  nmsg.ifaddr = ifaddr;
  nmsg.area_id = area_id;

  return msg_new (MSG_NEW_IF, &nmsg, seqnr, sizeof (struct msg_new_if));
}