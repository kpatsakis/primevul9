static int parse_flow_mask_nlattrs(const struct nlattr *attr,
				   const struct nlattr *a[], u64 *attrsp,
				   bool log)
{
	return __parse_flow_nlattrs(attr, a, attrsp, log, true);
}