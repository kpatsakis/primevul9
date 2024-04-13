static bool request_timed_out(struct timeval request_time,
			      struct timeval timeout)
{
	struct timeval now, end_time;
	GetTimeOfDay(&now);
	end_time = timeval_sum(&request_time, &timeout);
	return (timeval_compare(&end_time, &now) < 0);
}