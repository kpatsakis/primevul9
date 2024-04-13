str_toupper_z(const char *buff, Oid collid)
{
	return str_toupper(buff, strlen(buff), collid);
}