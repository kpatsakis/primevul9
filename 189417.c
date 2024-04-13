void complete_v1_state_transition(struct msg_digest *md, stf_status result)
{
	passert(md != NULL);

	/* handle oddball/meta results now */

	/*
	 * statistics; lump all FAILs together
	 *
	 * Fun fact: using min() stupidly fails (at least in GCC 8.1.1 with -Werror=sign-compare)
	 * error: comparison of integer expressions of different signedness: `stf_status' {aka `enum <anonymous>'} and `int'
	 */
	pstats(ike_stf, PMIN(result, STF_FAIL));

	DBG(DBG_CONTROL,
	    DBG_log("complete v1 state transition with %s",
		    result > STF_FAIL ?
		    enum_name(&ikev1_notify_names, result - STF_FAIL) :
		    enum_name(&stf_status_names, result)));

	switch (result) {
	case STF_SUSPEND:
		set_cur_state(md->st);	/* might have changed */
		/*
		 * If this transition was triggered by an incoming
		 * packet, save it.
		 *
		 * XXX: some initiator code creates a fake MD (there
		 * isn't a real one); save that as well.
		 */
		suspend_any_md(md->st, md);
		return;
	case STF_IGNORE:
		return;
	default:
		break;
	}

	/* safe to refer to *md */

	enum state_kind from_state = md->v1_from_state;

	struct state *st = md->st;
	set_cur_state(st); /* might have changed */

	passert(st != NULL);
	pexpect(!state_is_busy(st));

	if (result > STF_OK) {
		linux_audit_conn(md->st, IS_IKE_SA_ESTABLISHED(md->st) ? LAK_CHILD_FAIL : LAK_PARENT_FAIL);
	}

	switch (result) {
	case STF_OK:
	{
		/* advance the state */
		const struct state_v1_microcode *smc = md->smc;

		DBG(DBG_CONTROL, DBG_log("doing_xauth:%s, t_xauth_client_done:%s",
			bool_str(st->st_oakley.doing_xauth),
			bool_str(st->hidden_variables.st_xauth_client_done)));

		/* accept info from VID because we accept this message */

		/*
		 * Most of below VIDs only appear Main/Aggr mode, not Quick mode,
		 * so why are we checking them for each state transition?
		 */

		if (md->fragvid) {
			dbg("peer supports fragmentation");
			st->st_seen_fragvid = TRUE;
		}

		if (md->dpd) {
			dbg("peer supports DPD");
			st->hidden_variables.st_peer_supports_dpd = TRUE;
			if (dpd_active_locally(st)) {
				dbg("DPD is configured locally");
			}
		}

		/* If state has VID_NORTEL, import it to activate workaround */
		if (md->nortel) {
			dbg("peer requires Nortel Contivity workaround");
			st->st_seen_nortel_vid = TRUE;
		}

		if (!st->st_v1_msgid.reserved &&
		    IS_CHILD_SA(st) &&
		    st->st_v1_msgid.id != v1_MAINMODE_MSGID) {
			struct state *p1st = state_with_serialno(
				st->st_clonedfrom);

			if (p1st != NULL) {
				/* do message ID reservation */
				reserve_msgid(p1st, st->st_v1_msgid.id);
			}

			st->st_v1_msgid.reserved = TRUE;
		}

		dbg("IKEv1: transition from state %s to state %s",
		    finite_states[from_state]->name,
		    finite_states[smc->next_state]->name);

		change_state(st, smc->next_state);

		/*
		 * XAUTH negotiation without ModeCFG cannot follow the regular
		 * state machine change as it cannot be determined if the CFG
		 * payload is "XAUTH OK, no ModeCFG" or "XAUTH OK, expect
		 * ModeCFG". To the smc, these two cases look identical. So we
		 * have an ad hoc state change here for the case where
		 * we have XAUTH but not ModeCFG. We move it to the established
		 * state, so the regular state machine picks up the Quick Mode.
		 */
		if (st->st_connection->spd.this.xauth_client &&
		    st->hidden_variables.st_xauth_client_done &&
		    !st->st_connection->spd.this.modecfg_client &&
		    st->st_state->kind == STATE_XAUTH_I1)
		{
			bool aggrmode = LHAS(st->st_connection->policy, POLICY_AGGRESSIVE_IX);

			libreswan_log("XAUTH completed; ModeCFG skipped as per configuration");
			change_state(st, aggrmode ? STATE_AGGR_I2 : STATE_MAIN_I4);
			st->st_v1_msgid.phase15 = v1_MAINMODE_MSGID;
		}

		/* Schedule for whatever timeout is specified */
		if (!md->event_already_set) {
			/*
			 * This md variable is hardly ever set.
			 * Only deals with v1 <-> v2 switching
			 * which will be removed in the near future anyway
			 * (PW 2017 Oct 8)
			 */
			DBG(DBG_CONTROL, DBG_log("event_already_set, deleting event"));
			/*
			 * Delete previous retransmission event.
			 * New event will be scheduled below.
			 */
			delete_event(st);
			clear_retransmits(st);
		}

		/* Delete IKE fragments */
		free_v1_message_queues(st);

		/* scrub the previous packet exchange */
		free_chunk_content(&st->st_v1_rpacket);
		free_chunk_content(&st->st_v1_tpacket);

		/* in aggressive mode, there will be no reply packet in transition
		 * from STATE_AGGR_R1 to STATE_AGGR_R2
		 */
		if (nat_traversal_enabled && st->st_connection->ikev1_natt != NATT_NONE) {
			/* adjust our destination port if necessary */
			nat_traversal_change_port_lookup(md, st);
			v1_maybe_natify_initiator_endpoints(st, HERE);
		}

		/*
		 * Save both the received packet, and this
		 * state-transition.
		 *
		 * Only when the (last) state transition was a "reply"
		 * should a duplicate packet trigger a retransmit
		 * (else they get discarded).
		 *
		 * XXX: .st_state .fs_flags & SMF_REPLY can't
		 * be used because it contains flags for the new state
		 * not the old-to-new state transition.
		 */
		remember_received_packet(st, md);
		st->st_v1_last_transition = md->smc;

		/* if requested, send the new reply packet */
		if (smc->flags & SMF_REPLY) {
			endpoint_buf b;
			endpoint_buf b2;
			pexpect_st_local_endpoint(st);
			dbg("sending reply packet to %s (from %s)",
			    str_endpoint(&st->st_remote_endpoint, &b),
			    str_endpoint(&st->st_interface->local_endpoint, &b2));

			close_output_pbs(&reply_stream); /* good form, but actually a no-op */

			if (st->st_state->kind == STATE_MAIN_R2 &&
				impair.send_no_main_r2) {
				/* record-only so we propely emulate packet drop */
				record_outbound_v1_ike_msg(st, &reply_stream,
							   finite_states[from_state]->name);
				libreswan_log("IMPAIR: Skipped sending STATE_MAIN_R2 response packet");
			} else {
				record_and_send_v1_ike_msg(st, &reply_stream,
							   finite_states[from_state]->name);
			}
		}

		/* Schedule for whatever timeout is specified */
		if (!md->event_already_set) {
			DBG(DBG_CONTROL, DBG_log("!event_already_set at reschedule"));
			intmax_t delay_ms; /* delay is in milliseconds here */
			enum event_type kind = smc->timeout_event;
			bool agreed_time = FALSE;
			struct connection *c = st->st_connection;

			/* fixup in case of state machine jump for xauth without modecfg */
			if (c->spd.this.xauth_client &&
			    st->hidden_variables.st_xauth_client_done &&
			    !c->spd.this.modecfg_client &&
			    (st->st_state->kind == STATE_MAIN_I4 || st->st_state->kind == STATE_AGGR_I2))
			{
				DBG(DBG_CONTROL, DBG_log("fixup XAUTH without ModeCFG event from EVENT_RETRANSMIT to EVENT_SA_REPLACE"));
				kind = EVENT_SA_REPLACE;
			}

			switch (kind) {
			case EVENT_RETRANSMIT: /* Retransmit packet */
				start_retransmits(st);
				break;

			case EVENT_SA_REPLACE: /* SA replacement event */
				if (IS_PHASE1(st->st_state->kind) ||
				    IS_PHASE15(st->st_state->kind)) {
					/* Note: we will defer to the "negotiated" (dictated)
					 * lifetime if we are POLICY_DONT_REKEY.
					 * This allows the other side to dictate
					 * a time we would not otherwise accept
					 * but it prevents us from having to initiate
					 * rekeying.  The negative consequences seem
					 * minor.
					 */
					delay_ms = deltamillisecs(c->sa_ike_life_seconds);
					if ((c->policy & POLICY_DONT_REKEY) ||
					    delay_ms >= deltamillisecs(st->st_oakley.life_seconds))
					{
						agreed_time = TRUE;
						delay_ms = deltamillisecs(st->st_oakley.life_seconds);
					}
				} else {
					/* Delay is min of up to four things:
					 * each can limit the lifetime.
					 */
					time_t delay = deltasecs(c->sa_ipsec_life_seconds);

#define clamp_delay(trans) { \
		if (st->trans.present && \
		    delay >= deltasecs(st->trans.attrs.life_seconds)) { \
			agreed_time = TRUE; \
			delay = deltasecs(st->trans.attrs.life_seconds); \
		} \
	}
					clamp_delay(st_ah);
					clamp_delay(st_esp);
					clamp_delay(st_ipcomp);
					delay_ms = delay * 1000;
#undef clamp_delay
				}

				/* By default, we plan to rekey.
				 *
				 * If there isn't enough time to rekey, plan to
				 * expire.
				 *
				 * If we are --dontrekey, a lot more rules apply.
				 * If we are the Initiator, use REPLACE_IF_USED.
				 * If we are the Responder, and the dictated time
				 * was unacceptable (too large), plan to REPLACE
				 * (the only way to ratchet down the time).
				 * If we are the Responder, and the dictated time
				 * is acceptable, plan to EXPIRE.
				 *
				 * Important policy lies buried here.
				 * For example, we favour the initiator over the
				 * responder by making the initiator start rekeying
				 * sooner.  Also, fuzz is only added to the
				 * initiator's margin.
				 *
				 * Note: for ISAKMP SA, we let the negotiated
				 * time stand (implemented by earlier logic).
				 */
				if (agreed_time &&
				    (c->policy & POLICY_DONT_REKEY)) {
					kind = (smc->flags & SMF_INITIATOR) ?
					       EVENT_v1_SA_REPLACE_IF_USED :
					       EVENT_SA_EXPIRE;
				}
				if (kind != EVENT_SA_EXPIRE) {
					time_t marg =
						deltasecs(c->sa_rekey_margin);

					if (smc->flags & SMF_INITIATOR) {
						marg += marg *
							c->sa_rekey_fuzz /
							100.E0 *
							(rand() /
							 (RAND_MAX + 1.E0));
					} else {
						marg /= 2;
					}

					if (delay_ms > marg * 1000) {
						delay_ms -= marg * 1000;
						st->st_replace_margin = deltatime(marg);
					} else {
						kind = EVENT_SA_EXPIRE;
					}
				}
				/* XXX: DELAY_MS should be a deltatime_t */
				event_schedule(kind, deltatime_ms(delay_ms), st);
				break;

			case EVENT_SO_DISCARD:
				event_schedule(EVENT_SO_DISCARD, c->r_timeout, st);
				break;

			default:
				bad_case(kind);
			}
		}

		/* tell whack and log of progress */
		{
			enum rc_type w;
			void (*log_details)(struct lswlog *buf, struct state *st);

			if (IS_IPSEC_SA_ESTABLISHED(st)) {
				pstat_sa_established(st);
				log_details = lswlog_child_sa_established;
				w = RC_SUCCESS; /* log our success */
			} else if (IS_ISAKMP_SA_ESTABLISHED(st->st_state)) {
				pstat_sa_established(st);
				log_details = lswlog_ike_sa_established;
				w = RC_SUCCESS; /* log our success */
			} else {
				log_details = NULL;
				w = RC_NEW_V1_STATE + st->st_state->kind;
			}

			passert(st->st_state->kind < STATE_IKEv1_ROOF);

			/* tell whack and logs our progress */
			LSWLOG_RC(w, buf) {
				lswlogf(buf, "%s: %s", st->st_state->name,
					st->st_state->story);
				/* document SA details for admin's pleasure */
				if (log_details != NULL) {
					log_details(buf, st);
				}
			}
		}

		/*
		 * make sure that a DPD event gets created for a new phase 1
		 * SA.
		 * Why do we need a DPD event on an IKE SA???
		 */
		if (IS_ISAKMP_SA_ESTABLISHED(st->st_state)) {
			if (dpd_init(st) != STF_OK) {
				loglog(RC_LOG_SERIOUS, "DPD initialization failed - continuing without DPD");
			}
		}

		/* Special case for XAUTH server */
		if (st->st_connection->spd.this.xauth_server) {
			if (st->st_oakley.doing_xauth &&
			    IS_ISAKMP_SA_ESTABLISHED(st->st_state)) {
				DBG(DBG_CONTROLMORE|DBG_XAUTH,
				    DBG_log("XAUTH: Sending XAUTH Login/Password Request"));
				event_schedule(EVENT_v1_SEND_XAUTH,
					       deltatime_ms(EVENT_v1_SEND_XAUTH_DELAY_MS),
					       st);
				break;
			}
		}

		/*
		 * for XAUTH client, we are also done, because we need to
		 * stay in this state, and let the server query us
		 */
		if (!IS_QUICK(st->st_state->kind) &&
		    st->st_connection->spd.this.xauth_client &&
		    !st->hidden_variables.st_xauth_client_done) {
			DBG(DBG_CONTROL,
			    DBG_log("XAUTH client is not yet authenticated"));
			break;
		}

		/*
		 * when talking to some vendors, we need to initiate a mode
		 * cfg request to get challenged, but there is also an
		 * override in the form of a policy bit.
		 */
		DBG(DBG_CONTROL,
		    DBG_log("modecfg pull: %s policy:%s %s",
			    (st->quirks.modecfg_pull_mode ?
			     "quirk-poll" : "noquirk"),
			    (st->st_connection->policy & POLICY_MODECFG_PULL) ?
			    "pull" : "push",
			    (st->st_connection->spd.this.modecfg_client ?
			     "modecfg-client" : "not-client")));

		if (st->st_connection->spd.this.modecfg_client &&
		    IS_ISAKMP_SA_ESTABLISHED(st->st_state) &&
		    (st->quirks.modecfg_pull_mode ||
		     st->st_connection->policy & POLICY_MODECFG_PULL) &&
		    !st->hidden_variables.st_modecfg_started) {
			DBG(DBG_CONTROL,
			    DBG_log("modecfg client is starting due to %s",
				    st->quirks.modecfg_pull_mode ? "quirk" :
				    "policy"));
			modecfg_send_request(st);
			break;
		}

		/* Should we set the peer's IP address regardless? */
		if (st->st_connection->spd.this.modecfg_server &&
		    IS_ISAKMP_SA_ESTABLISHED(st->st_state) &&
		    !st->hidden_variables.st_modecfg_vars_set &&
		    !(st->st_connection->policy & POLICY_MODECFG_PULL)) {
			change_state(st, STATE_MODE_CFG_R1);
			set_cur_state(st);
			libreswan_log("Sending MODE CONFIG set");
			/*
			 * ??? we ignore the result of modecfg.
			 * But surely, if it fails, we ought to terminate this exchange.
			 * What do the RFCs say?
			 */
			modecfg_start_set(st);
			break;
		}

		/*
		 * If we are the responder and the client is in "Contivity mode",
		 * we need to initiate Quick mode
		 */
		if (!(smc->flags & SMF_INITIATOR) &&
		    IS_MODE_CFG_ESTABLISHED(st->st_state) &&
		    (st->st_seen_nortel_vid)) {
			libreswan_log("Nortel 'Contivity Mode' detected, starting Quick Mode");
			change_state(st, STATE_MAIN_R3); /* ISAKMP is up... */
			set_cur_state(st);
			quick_outI1(st->st_whack_sock, st, st->st_connection,
				    st->st_connection->policy, 1, SOS_NOBODY,
				    NULL /* Setting NULL as this is responder and will not have sec ctx from a flow*/
				    );
			break;
		}

		/* wait for modecfg_set */
		if (st->st_connection->spd.this.modecfg_client &&
		    IS_ISAKMP_SA_ESTABLISHED(st->st_state) &&
		    !st->hidden_variables.st_modecfg_vars_set) {
			DBG(DBG_CONTROL,
			    DBG_log("waiting for modecfg set from server"));
			break;
		}

		DBG(DBG_CONTROL,
		    DBG_log("phase 1 is done, looking for phase 2 to unpend"));

		if (smc->flags & SMF_RELEASE_PENDING_P2) {
			/* Initiate any Quick Mode negotiations that
			 * were waiting to piggyback on this Keying Channel.
			 *
			 * ??? there is a potential race condition
			 * if we are the responder: the initial Phase 2
			 * message might outrun the final Phase 1 message.
			 *
			 * so, instead of actually sending the traffic now,
			 * we schedule an event to do so.
			 *
			 * but, in fact, quick_mode will enqueue a cryptographic operation
			 * anyway, which will get done "later" anyway, so maybe it is just fine
			 * as it is.
			 *
			 */
			unpend(pexpect_ike_sa(st), NULL);
		}

		if (IS_ISAKMP_SA_ESTABLISHED(st->st_state) ||
		    IS_IPSEC_SA_ESTABLISHED(st))
			release_any_whack(st, HERE, "IKEv1 transitions finished");

		if (IS_QUICK(st->st_state->kind))
			break;

		break;
	}

	case STF_INTERNAL_ERROR:
		/* update the previous packet history */
		remember_received_packet(st, md);
		log_state(RC_INTERNALERR + md->v1_note, st,
			  "state transition function for %s had internal error",
			  st->st_state->name);
		release_pending_whacks(st, "internal error");
		break;

	case STF_FATAL:
		passert(st != NULL);
		/* update the previous packet history */
		remember_received_packet(st, md);
		log_state(RC_FATAL, st, "encountered fatal error in state %s",
			  st->st_state->name);
#ifdef HAVE_NM
		if (st->st_connection->remotepeertype == CISCO &&
		    st->st_connection->nmconfigured) {
			if (!do_command(st->st_connection,
					&st->st_connection->spd,
					"disconnectNM", st))
				DBG(DBG_CONTROL,
				    DBG_log("sending disconnect to NM failed, you may need to do it manually"));
		}
#endif
		release_pending_whacks(st, "fatal error");
		delete_state(st);
		md->st = st = NULL;
		break;

	default:        /* a shortcut to STF_FAIL, setting md->note */
		passert(result > STF_FAIL);
		md->v1_note = result - STF_FAIL;
		/* FALL THROUGH */
	case STF_FAIL:
	{
		/* As it is, we act as if this message never happened:
		 * whatever retrying was in place, remains in place.
		 */
		/*
		 * Try to convert the notification into a non-NULL
		 * string.  For NOTHING_WRONG, be vague (at the time
		 * of writing the enum_names didn't contain
		 * NOTHING_WRONG, and even if it did "nothing wrong"
		 * wouldn't exactly help here :-).
		 */
		const char *notify_name = (md->v1_note == NOTHING_WRONG ? "failed" :
					   enum_name(&ikev1_notify_names, md->v1_note));
		if (notify_name == NULL) {
			notify_name = "internal error";
		}
		/*
		 * ??? why no call of remember_received_packet?
		 * Perhaps because the message hasn't been authenticated?
		 * But then then any duplicate would lose too, I would think.
		 */

		if (md->v1_note != NOTHING_WRONG) {
			/* this will log */
			SEND_NOTIFICATION(md->v1_note);
		} else {
			/* XXX: why whack only? */
			log_state(WHACK_STREAM | (RC_NOTIFICATION + md->v1_note), st,
				  "state transition failed: %s", notify_name);
		}

		dbg("state transition function for %s failed: %s",
		    st->st_state->name, notify_name);

#ifdef HAVE_NM
		if (st->st_connection->remotepeertype == CISCO &&
		    st->st_connection->nmconfigured) {
			if (!do_command(st->st_connection,
					&st->st_connection->spd,
					"disconnectNM", st))
				DBG(DBG_CONTROL,
				    DBG_log("sending disconnect to NM failed, you may need to do it manually"));
		}
#endif
		if (IS_QUICK(st->st_state->kind)) {
			delete_state(st);
			/* wipe out dangling pointer to st */
			md->st = NULL;
		}
		break;
	}
	}
}