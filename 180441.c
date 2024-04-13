new_msg_ism_change (u_int32_t seqnr, struct in_addr ifaddr,
		    struct in_addr area_id, u_char status)
{
  struct msg_ism_change imsg;

  imsg.ifaddr = ifaddr;
  imsg.area_id = area_id;
  imsg.status = status;
  memset (&imsg.pad, 0, sizeof (imsg.pad));

  return msg_new (MSG_ISM_CHANGE, &imsg, seqnr,
		  sizeof (struct msg_ism_change));
}