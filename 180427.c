msg_write (int fd, struct msg *msg)
{
  u_char buf[OSPF_API_MAX_MSG_SIZE];
  int l;
  int wlen;

  assert (msg);
  assert (msg->s);

  /* Length of message including header */
  l = sizeof (struct apimsghdr) + ntohs (msg->hdr.msglen);

  /* Make contiguous memory buffer for message */
  memcpy (buf, &msg->hdr, sizeof (struct apimsghdr));
  memcpy (buf + sizeof (struct apimsghdr), STREAM_DATA (msg->s),
	  ntohs (msg->hdr.msglen));

  wlen = writen (fd, buf, l);
  if (wlen < 0)
    {
      zlog_warn ("msg_write: writen %s", safe_strerror (errno));
      return -1;
    }
  else if (wlen == 0)
    {
      zlog_warn ("msg_write: Connection closed by peer");
      return -1;
    }
  else if (wlen != l)
    {
      zlog_warn ("msg_write: Cannot write API message");
      return -1;
    }
  return 0;
}