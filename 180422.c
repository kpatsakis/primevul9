msg_dup (struct msg *msg)
{
  struct msg *new;

  assert (msg);

  new = msg_new (msg->hdr.msgtype, STREAM_DATA (msg->s),
		 ntohl (msg->hdr.msgseq), ntohs (msg->hdr.msglen));
  return new;
}