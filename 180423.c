msg_new (u_char msgtype, void *msgbody, u_int32_t seqnum, u_int16_t msglen)
{
  struct msg *new;

  new = XCALLOC (MTYPE_OSPF_API_MSG, sizeof (struct msg));

  new->hdr.version = OSPF_API_VERSION;
  new->hdr.msgtype = msgtype;
  new->hdr.msglen = htons (msglen);
  new->hdr.msgseq = htonl (seqnum);

  new->s = stream_new (msglen);
  assert (new->s);
  stream_put (new->s, msgbody, msglen);

  return new;
}