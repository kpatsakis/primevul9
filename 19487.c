const char *jsP_aststring(enum js_AstType type)
{
	if (type < nelem(astname)-1)
		return astname[type];
	return "<unknown>";
}