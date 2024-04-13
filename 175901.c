static void set_ns_and_type_ex(xmlNodePtr node, char *ns, char *type)
{
	smart_str nstype = {0};
	get_type_str(node, ns, type, &nstype);
	set_xsi_type(node, nstype.c);
	smart_str_free(&nstype);
}