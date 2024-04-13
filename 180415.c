new_msg_nsm_change (u_int32_t seqnr, struct in_addr ifaddr,
		    struct in_addr nbraddr,
		    struct in_addr router_id, u_char status)
{
  struct msg_nsm_change nmsg;

  nmsg.ifaddr = ifaddr;
  nmsg.nbraddr = nbraddr;
  nmsg.router_id = router_id;
  nmsg.status = status;
  memset (&nmsg.pad, 0, sizeof (nmsg.pad));

  return msg_new (MSG_NSM_CHANGE, &nmsg, seqnr,
		  sizeof (struct msg_nsm_change));
}