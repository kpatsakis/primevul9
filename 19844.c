u32 ovs_nla_get_ufid_flags(const struct nlattr *attr)
{
	return attr ? nla_get_u32(attr) : 0;
}