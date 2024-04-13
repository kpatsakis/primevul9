static bool cache_check_is_valid(struct cache_data *data,
				time_t current_time)
{
	if (!data)
		return false;

	if (data->cache_until < current_time)
		return false;

	return true;
}