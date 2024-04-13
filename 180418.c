msg_get_seq (struct msg *msg)
{
  assert (msg);
  return ntohl (msg->hdr.msgseq);
}