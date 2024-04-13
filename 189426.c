void jam_v1_transition(jambuf_t *buf, const struct state_v1_microcode *transition)
{
	if (transition == NULL) {
		jam(buf, "NULL");
	} else {
		jam(buf, "%s->%s",
		    finite_states[transition->state]->short_name,
		    finite_states[transition->next_state]->short_name);
	}
}