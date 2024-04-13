new_msg_reply (u_int32_t seqnr, u_char rc)
{
  struct msg *msg;
  struct msg_reply rmsg;

  /* Set return code */
  rmsg.errcode = rc;
  memset (&rmsg.pad, 0, sizeof (rmsg.pad));

  msg = msg_new (MSG_REPLY, &rmsg, seqnr, sizeof (struct msg_reply));

  return msg;
}