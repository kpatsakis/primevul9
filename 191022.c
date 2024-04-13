struct idle_event *event_add_idle(struct event_context *event_ctx,
				  TALLOC_CTX *mem_ctx,
				  struct timeval interval,
				  const char *name,
				  bool (*handler)(const struct timeval *now,
						  void *private_data),
				  void *private_data)
{
	struct idle_event *result;
	struct timeval now = timeval_current();

	result = TALLOC_P(mem_ctx, struct idle_event);
	if (result == NULL) {
		DEBUG(0, ("talloc failed\n"));
		return NULL;
	}

	result->interval = interval;
	result->handler = handler;
	result->private_data = private_data;

	if (!(result->name = talloc_asprintf(result, "idle_evt(%s)", name))) {
		DEBUG(0, ("talloc failed\n"));
		TALLOC_FREE(result);
		return NULL;
	}

	result->te = event_add_timed(event_ctx, result,
				     timeval_sum(&now, &interval),
				     smbd_idle_event_handler, result);
	if (result->te == NULL) {
		DEBUG(0, ("event_add_timed failed\n"));
		TALLOC_FREE(result);
		return NULL;
	}

	DEBUG(10,("event_add_idle: %s %p\n", result->name, result->te));
	return result;
}