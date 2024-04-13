struct event_constraint *intel_pebs_constraints(struct perf_event *event)
{
	struct event_constraint *c;

	if (!event->attr.precise_ip)
		return NULL;

	if (x86_pmu.pebs_constraints) {
		for_each_event_constraint(c, x86_pmu.pebs_constraints) {
			if (constraint_match(c, event->hw.config)) {
				event->hw.flags |= c->flags;
				return c;
			}
		}
	}

	/*
	 * Extended PEBS support
	 * Makes the PEBS code search the normal constraints.
	 */
	if (x86_pmu.flags & PMU_FL_PEBS_ALL)
		return NULL;

	return &emptyconstraint;
}