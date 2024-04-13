static stf_status informational(struct state *st, struct msg_digest *md)
{
	/*
	 * XXX: Danger: ST is deleted midway through this function.
	 */
	pexpect(st == md->st);
	st = md->st;    /* may be NULL */

	struct payload_digest *const n_pld = md->chain[ISAKMP_NEXT_N];

	/* If the Notification Payload is not null... */
	if (n_pld != NULL) {
		pb_stream *const n_pbs = &n_pld->pbs;
		struct isakmp_notification *const n =
			&n_pld->payload.notification;

		/* Switch on Notification Type (enum) */
		/* note that we _can_ get notification payloads unencrypted
		 * once we are at least in R3/I4.
		 * and that the handler is expected to treat them suspiciously.
		 */
		dbg("processing informational %s (%d)",
		    enum_name(&ikev1_notify_names, n->isan_type),
		    n->isan_type);

		pstats(ikev1_recv_notifies_e, n->isan_type);

		switch (n->isan_type) {
		/*
		 * We answer DPD probes even if they claimed not to support
		 * Dead Peer Detection.
		 * We would have to send some kind of reply anyway to prevent
		 * a retransmit, so rather then send an error, we might as
		 * well just send a DPD reply
		 */
		case R_U_THERE:
			if (st == NULL) {
				plog_md(md, "received bogus  R_U_THERE informational message");
				return STF_IGNORE;
			}
			return dpd_inI_outR(st, n, n_pbs);

		case R_U_THERE_ACK:
			if (st == NULL) {
				plog_md(md, "received bogus R_U_THERE_ACK informational message");
				return STF_IGNORE;
			}
			return dpd_inR(st, n, n_pbs);

		case PAYLOAD_MALFORMED:
			if (st != NULL) {
				st->hidden_variables.st_malformed_received++;

				log_state(RC_LOG, st, "received %u malformed payload notifies",
					  st->hidden_variables.st_malformed_received);

				if (st->hidden_variables.st_malformed_sent >
				    MAXIMUM_MALFORMED_NOTIFY / 2 &&
				    ((st->hidden_variables.st_malformed_sent +
				      st->hidden_variables.
				      st_malformed_received) >
				     MAXIMUM_MALFORMED_NOTIFY)) {
					log_state(RC_LOG, st, "too many malformed payloads (we sent %u and received %u",
						  st->hidden_variables.st_malformed_sent,
						  st->hidden_variables.st_malformed_received);
					delete_state(st);
					md->st = st = NULL;
				}
			}

			return STF_IGNORE;

		case ISAKMP_N_CISCO_LOAD_BALANCE:
			/*
			 * ??? what the heck is in the payload?
			 * We take the peer's new IP address from the last 4 octets.
			 * Is anything else possible?  Expected?  Documented?
			 */
			if (st == NULL || !IS_ISAKMP_SA_ESTABLISHED(st->st_state)) {
				plog_md(md, "ignoring ISAKMP_N_CISCO_LOAD_BALANCE Informational Message with for unestablished state.");
			} else if (pbs_left(n_pbs) < 4) {
				log_state(RC_LOG_SERIOUS, st,
					  "ignoring ISAKMP_N_CISCO_LOAD_BALANCE Informational Message without IPv4 address");
			} else {
				/*
				 * Copy (not cast) the last 4 bytes
				 * (size of an IPv4) address from the
				 * end of the notification into IN
				 * (can't cast as can't assume that
				 * ->roof-4 is correctly aligned).
				 */
				struct in_addr in;
				memcpy(&in, n_pbs->roof - sizeof(in), sizeof(in));
				ip_address new_peer = address_from_in_addr(&in);

				/* is all zeros? */
				if (address_is_any(&new_peer)) {
					ipstr_buf b;

					log_state(RC_LOG_SERIOUS, st,
						  "ignoring ISAKMP_N_CISCO_LOAD_BALANCE Informational Message with invalid IPv4 address %s",
						  ipstr(&new_peer, &b));
					return FALSE; /* XXX: STF_*? */
				}

				/* Saving connection name and whack sock id */
				const char *tmp_name = st->st_connection->name;
				struct fd *tmp_whack_sock = dup_any(st->st_whack_sock);

				/* deleting ISAKMP SA with the current remote peer */
				delete_state(st);
				md->st = st = NULL;

				/* to find and store the connection associated with tmp_name */
				/* ??? how do we know that tmp_name hasn't been freed? */
				struct connection *tmp_c = conn_by_name(tmp_name, false/*!strict*/);

				if (DBGP(DBG_BASE)) {
					address_buf npb;
					DBG_log("new peer address: %s",
						str_address(&new_peer, &npb));

					/* Current remote peer info */
					int count_spd = 1;
					for (const struct spd_route *tmp_spd = &tmp_c->spd;
					     tmp_spd != NULL; tmp_spd = tmp_spd->spd_next) {
						address_buf b;
						endpoint_buf e;

						DBG_log("spd route number: %d",
							count_spd++);

						/**that info**/
						DBG_log("that id kind: %d",
							tmp_spd->that.id.kind);
						DBG_log("that id ipaddr: %s",
							str_address(&tmp_spd->that.id.ip_addr, &b));
						if (tmp_spd->that.id.name.ptr != NULL) {
							DBG_dump_hunk("that id name",
								      tmp_spd->that.id. name);
						}
						DBG_log("that host_addr: %s",
							str_endpoint(&tmp_spd->that.host_addr, &e));
						DBG_log("that nexthop: %s",
							str_address(&tmp_spd->that.host_nexthop, &b));
						DBG_log("that srcip: %s",
							str_address(&tmp_spd->that.host_srcip, &b));
						selector_buf sb;
						DBG_log("that client: %s",
							str_selector(&tmp_spd->that.client, &sb));
						DBG_log("that has_client: %d",
							tmp_spd->that.has_client);
						DBG_log("that has_client_wildcard: %d",
							tmp_spd->that.has_client_wildcard);
						DBG_log("that has_port_wildcard: %d",
							tmp_spd->that.has_port_wildcard);
						DBG_log("that has_id_wildcards: %d",
							tmp_spd->that.has_id_wildcards);
					}

					if (tmp_c->interface != NULL) {
						endpoint_buf b;
						DBG_log("Current interface_addr: %s",
							str_endpoint(&tmp_c->interface->local_endpoint, &b));
					}
				}

				/* save peer's old address for comparison purposes */
				ip_address old_addr = tmp_c->spd.that.host_addr;

				/* update peer's address */
				tmp_c->spd.that.host_addr = new_peer;

				/* Modifying connection info to store the redirected remote peer info */
				dbg("Old host_addr_name : %s", tmp_c->spd.that.host_addr_name);
				tmp_c->spd.that.host_addr_name = NULL;

				/* ??? do we know the id.kind has an ip_addr? */
				tmp_c->spd.that.id.ip_addr = new_peer;

				/* update things that were the old peer */
				ipstr_buf b;
				if (sameaddr(&tmp_c->spd.this.host_nexthop,
					     &old_addr)) {
					if (DBGP(DBG_BASE)) {
						DBG_log("this host's next hop %s was the same as the old remote addr",
							ipstr(&old_addr, &b));
						DBG_log("changing this host's next hop to %s",
							ipstr(&new_peer, &b));
					}
					tmp_c->spd.this.host_nexthop = new_peer;
				}

				if (sameaddr(&tmp_c->spd.that.host_srcip,
					     &old_addr)) {
					if (DBGP(DBG_BASE)) {
						DBG_log("Old that host's srcip %s was the same as the old remote addr",
							ipstr(&old_addr, &b));
						DBG_log("changing that host's srcip to %s",
							ipstr(&new_peer, &b));
					}
					tmp_c->spd.that.host_srcip = new_peer;
				}

				if (sameaddr(&tmp_c->spd.that.client.addr,
					     &old_addr)) {
					if (DBGP(DBG_BASE)) {
						DBG_log("Old that client ip %s was the same as the old remote address",
							ipstr(&old_addr, &b));
						DBG_log("changing that client's ip to %s",
							ipstr(&new_peer, &b));
					}
					tmp_c->spd.that.client.addr = new_peer;
				}

				/*
				 * ??? is this wise?  This may changes
				 * a lot of other connections.
				 *
				 * XXX:
				 *
				 * As for the old code, preserve the
				 * existing port.  NEW_PEER, an
				 * address, doesn't have a port and
				 * presumably the port wasn't
				 * updated(?).
				 */
				tmp_c->host_pair->remote = endpoint(&new_peer,
								    endpoint_hport(&tmp_c->host_pair->remote));

				/* Initiating connection to the redirected peer */
				initiate_connections_by_name(tmp_name, NULL,
							     tmp_whack_sock,
							     tmp_whack_sock == NULL/*guess*/);
				close_any(&tmp_whack_sock);
			}
			return STF_IGNORE;
		default:
		{
			struct logger logger = st != NULL ? *(st->st_logger) : MESSAGE_LOGGER(md);
			log_message(RC_LOG_SERIOUS, &logger,
				    "received and ignored notification payload: %s",
				    enum_name(&ikev1_notify_names, n->isan_type));
			return STF_IGNORE;
		}
		}
	} else {
		/* warn if we didn't find any Delete or Notify payload in packet */
		if (md->chain[ISAKMP_NEXT_D] == NULL) {
			struct logger logger = st != NULL ? *(st->st_logger) : MESSAGE_LOGGER(md);
			log_message(RC_LOG_SERIOUS, &logger,
				    "received and ignored empty informational notification payload");
		}
		return STF_IGNORE;
	}
}