static bool ikev1_duplicate(struct state *st, struct msg_digest *md)
{
	passert(st != NULL);
	if (st->st_v1_rpacket.ptr != NULL &&
	    st->st_v1_rpacket.len == pbs_room(&md->packet_pbs) &&
	    memeq(st->st_v1_rpacket.ptr, md->packet_pbs.start,
		  st->st_v1_rpacket.len)) {
		/*
		 * Duplicate.  Drop or retransmit?
		 *
		 * Only re-transmit when the last state transition
		 * (triggered by this packet the first time) included
		 * a reply.
		 *
		 * XXX: is SMF_RETRANSMIT_ON_DUPLICATE useful or
		 * correct?
		 */
		bool replied = (st->st_v1_last_transition != NULL &&
				(st->st_v1_last_transition->flags & SMF_REPLY));
		bool retransmit_on_duplicate =
			(st->st_state->flags & SMF_RETRANSMIT_ON_DUPLICATE);
		if (replied && retransmit_on_duplicate) {
			/*
			 * Transitions with EVENT_SO_DISCARD should
			 * always respond to re-transmits (why?); else
			 * cap.
			 */
			if (st->st_v1_last_transition->timeout_event == EVENT_SO_DISCARD ||
			    count_duplicate(st, MAXIMUM_v1_ACCEPTED_DUPLICATES)) {
				loglog(RC_RETRANSMISSION,
				       "retransmitting in response to duplicate packet; already %s",
				       st->st_state->name);
				resend_recorded_v1_ike_msg(st, "retransmit in response to duplicate");
			} else {
				loglog(RC_LOG_SERIOUS,
				       "discarding duplicate packet -- exhausted retransmission; already %s",
				       st->st_state->name);
			}
		} else {
			dbg("#%lu discarding duplicate packet; already %s; replied=%s retransmit_on_duplicate=%s",
			    st->st_serialno, st->st_state->name,
			    bool_str(replied), bool_str(retransmit_on_duplicate));
		}
		return true;
	}
	return false;
}