static void set_xsi_nil(xmlNodePtr node)
{
	set_ns_prop(node, XSI_NAMESPACE, "nil", "true");
}