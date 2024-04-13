static int unix_authorize(void *instance, REQUEST *request)
{
	return unix_getpw(instance, request, &request->config_items);
}