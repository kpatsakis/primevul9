str_initcap_z(const char *buff, Oid collid)
{
	return str_initcap(buff, strlen(buff), collid);
}