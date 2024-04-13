static bool should_fail_request(struct hd_struct *part, unsigned int bytes)
{
	return part->make_it_fail && should_fail(&fail_make_request, bytes);
}