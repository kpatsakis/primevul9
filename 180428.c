msg_free (struct msg *msg)
{
  if (msg->s)
    stream_free (msg->s);

  XFREE (MTYPE_OSPF_API_MSG, msg);
}