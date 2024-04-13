static void set_xsi_type(xmlNodePtr node, char *type)
{
	set_ns_prop(node, XSI_NAMESPACE, "type", type);
}