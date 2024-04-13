new_msg_delete_request (u_int32_t seqnum,
			struct in_addr area_id, u_char lsa_type,
			u_char opaque_type, u_int32_t opaque_id)
{
  struct msg_delete_request dmsg;
  dmsg.area_id = area_id;
  dmsg.lsa_type = lsa_type;
  dmsg.opaque_type = opaque_type;
  dmsg.opaque_id = htonl (opaque_id);
  memset (&dmsg.pad, 0, sizeof (dmsg.pad));

  return msg_new (MSG_DELETE_REQUEST, &dmsg, seqnum,
		  sizeof (struct msg_delete_request));
}