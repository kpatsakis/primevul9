msg_print (struct msg *msg)
{
  if (!msg)
    {
      zlog_debug ("msg_print msg=NULL!\n");
      return;
    }

#ifdef ORIGINAL_CODING
  zlog_debug
    ("msg=%p msgtype=%d msglen=%d msgseq=%d streamdata=%p streamsize=%lu\n",
     msg, msg->hdr.msgtype, ntohs (msg->hdr.msglen), ntohl (msg->hdr.msgseq),
     STREAM_DATA (msg->s), STREAM_SIZE (msg->s));
#else /* ORIGINAL_CODING */
  /* API message common header part. */
  zlog_debug
    ("API-msg [%s]: type(%d),len(%d),seq(%lu),data(%p),size(%zd)",
     ospf_api_typename (msg->hdr.msgtype), msg->hdr.msgtype, 
     ntohs (msg->hdr.msglen), (unsigned long) ntohl (msg->hdr.msgseq),
     STREAM_DATA (msg->s), STREAM_SIZE (msg->s));

  /* API message body part. */
#ifdef ndef
  /* Generic Hex/Ascii dump */
  DumpBuf (STREAM_DATA (msg->s), STREAM_SIZE (msg->s)); /* Sorry, deleted! */
#else /* ndef */
  /* Message-type dependent dump function. */
#endif /* ndef */

  return;
#endif /* ORIGINAL_CODING */
}