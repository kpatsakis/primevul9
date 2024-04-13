bool is_deferred_open_async(const void *ptr)
{
	const struct deferred_open_record *state = (const struct deferred_open_record *)ptr;

	return state->async_open;
}