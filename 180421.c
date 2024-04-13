new_msg_register_opaque_type (u_int32_t seqnum, u_char ltype, u_char otype)
{
  struct msg_register_opaque_type rmsg;

  rmsg.lsatype = ltype;
  rmsg.opaquetype = otype;
  memset (&rmsg.pad, 0, sizeof (rmsg.pad));

  return msg_new (MSG_REGISTER_OPAQUETYPE, &rmsg, seqnum,
		  sizeof (struct msg_register_opaque_type));
}