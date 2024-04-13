str_tolower_z(const char *buff, Oid collid)
{
	return str_tolower(buff, strlen(buff), collid);
}