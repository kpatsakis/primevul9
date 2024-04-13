static void set_ns_prop(xmlNodePtr node, char *ns, char *name, char *val)
{
	xmlSetNsProp(node, encode_add_ns(node, ns), BAD_CAST(name), BAD_CAST(val));
}