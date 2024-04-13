new_msg_ready_notify (u_int32_t seqnr, u_char lsa_type,
		      u_char opaque_type, struct in_addr addr)
{
  struct msg_ready_notify rmsg;

  rmsg.lsa_type = lsa_type;
  rmsg.opaque_type = opaque_type;
  memset (&rmsg.pad, 0, sizeof (rmsg.pad));
  rmsg.addr = addr;

  return msg_new (MSG_READY_NOTIFY, &rmsg, seqnr,
		  sizeof (struct msg_ready_notify));
}