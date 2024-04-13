void init_ikev1(void)
{
	DBGF(DBG_CONTROL, "checking IKEv1 state table");

	/*
	 * Fill in FINITE_STATES[].
	 *
	 * This is a hack until each finite-state is a separate object
	 * with corresponding edges (aka microcodes).
	 *
	 * XXX: Long term goal is to have a constant FINITE_STATES[]
	 * contain constant pointers and this static writeable array
	 * to just go away.
	 */
	for (enum state_kind kind = STATE_IKEv1_FLOOR; kind < STATE_IKEv1_ROOF; kind++) {
		/* fill in using static struct */
		const struct finite_state *fs = &v1_states[kind - STATE_IKEv1_FLOOR];
		passert(fs->kind == kind);
		passert(finite_states[kind] == NULL);
		finite_states[kind] = fs;
	}

	/*
	 * Go through the state transition table filling in details
	 * and checking for inconsistencies.
	 */
	for (const struct state_v1_microcode *t = v1_state_microcode_table;
	     t->state < STATE_IKEv1_ROOF; t++) {

		passert(t->state >= STATE_IKEv1_FLOOR);
		passert(t->state < STATE_IKEv1_ROOF);
		struct finite_state *from = &v1_states[t->state - STATE_IKEv1_FLOOR];

		/*
		 * Deal with next_state == STATE_UNDEFINED.
		 *
		 * XXX: STATE_UNDEFINED is used when a state
		 * transitions back to the same state; such
		 * transitions should instead explicitly specify that
		 * same state.
		 */
		enum state_kind next_state = (t->next_state == STATE_UNDEFINED ?
					      t->state : t->next_state);
		passert(STATE_IKEv1_FLOOR <= next_state &&
			next_state < STATE_IKEv1_ROOF);
		const struct finite_state *to = finite_states[next_state];
		passert(to != NULL);

		if (DBGP(DBG_BASE)) {
			if (from->nr_transitions == 0) {
				LSWLOG_DEBUG(buf) {
					lswlogs(buf, "  ");
					lswlog_finite_state(buf, from);
					lswlogs(buf, ":");
				}
			}
			DBG_log("    -> %s %s (%s)", to->short_name,
				enum_short_name(&timer_event_names,
						t->timeout_event),
				t->message);
		}

		/*
		 * Point .fs_v1_transitions at to the first entry in
		 * v1_state_microcode_table for that state.  All other
		 * transitions for that state should follow
		 * immediately after (or to put it another way, the
		 * previous transition's state should be the same as
		 * this).
		 */
		if (from->v1_transitions == NULL) {
			from->v1_transitions = t;
		} else {
			passert(t[-1].state == t->state);
		}
		from->nr_transitions++;

		if (t->message == NULL) {
			PEXPECT_LOG("transition %s -> %s missing .message",
				    from->short_name, to->short_name);
		}

		/*
		 * Copy (actually merge) the flags that apply to the
		 * state; and not the state transition.
		 *
		 * The original code used something like state
		 * .microcode .flags after the state transition had
		 * completed.  I.e., use the flags from a
		 * not-yet-taken potential future state transition and
		 * not the previous one.
		 *
		 * This is just trying to extact extract them and
		 * check they are consistent.
		 *
		 * XXX: this is confusing
		 *
		 * Should fs_flags and SMF_RETRANSMIT_ON_DUPLICATE
		 * should be replaced by SMF_RESPONDING in the
		 * transition flags?
		 *
		 * Or is this more like .fs_timeout_event which is
		 * always true of a state?
		 */
		if ((t->flags & from->flags) != from->flags) {
			DBGF(DBG_BASE, "transition %s -> %s (%s) missing flags 0x%"PRIxLSET,
			     from->short_name, to->short_name,
			     t->message, from->flags);
		}
		from->flags |= t->flags & SMF_RETRANSMIT_ON_DUPLICATE;

		if (!(t->flags & SMF_FIRST_ENCRYPTED_INPUT) &&
		    (t->flags & SMF_INPUT_ENCRYPTED) &&
		    t->processor != unexpected) {
			/*
			 * The first encrypted message carries
			 * authentication information so isn't
			 * applicable.  Other encrypted messages
			 * require integrity via the HASH payload.
			 */
			if (!(t->req_payloads & LELEM(ISAKMP_NEXT_HASH))) {
				PEXPECT_LOG("transition %s -> %s (%s) missing HASH payload",
					    from->short_name, to->short_name,
					    t->message);
			}
			if (t->hash_type == V1_HASH_NONE) {
				PEXPECT_LOG("transition %s -> %s (%s) missing HASH protection",
					    from->short_name, to->short_name,
					    t->message);
			}
		}
	}
}