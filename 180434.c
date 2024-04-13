msg_set_seq (struct msg *msg, u_int32_t seqnr)
{
  assert (msg);
  msg->hdr.msgseq = htonl (seqnr);
}