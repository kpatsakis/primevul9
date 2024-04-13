new_msg_originate_request (u_int32_t seqnum,
			   struct in_addr ifaddr,
			   struct in_addr area_id, struct lsa_header *data)
{
  struct msg_originate_request *omsg;
  int omsglen;
  char buf[OSPF_API_MAX_MSG_SIZE];

  omsg = (struct msg_originate_request *) buf;
  omsg->ifaddr = ifaddr;
  omsg->area_id = area_id;

  omsglen = ntohs (data->length);
  if (omsglen > sizeof (buf) - offsetof (struct msg_originate_request, data))
    omsglen = sizeof (buf) - offsetof (struct msg_originate_request, data);
  memcpy (&omsg->data, data, omsglen);
  omsglen += sizeof (struct msg_originate_request) - sizeof (struct lsa_header);

  return msg_new (MSG_ORIGINATE_REQUEST, omsg, seqnum, omsglen);
}