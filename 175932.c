static void set_ns_and_type(xmlNodePtr node, encodeTypePtr type)
{
	set_ns_and_type_ex(node, type->ns, type->type_str);
}