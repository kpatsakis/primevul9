static int validate_userspace(const struct nlattr *attr)
{
	static const struct nla_policy userspace_policy[OVS_USERSPACE_ATTR_MAX + 1] = {
		[OVS_USERSPACE_ATTR_PID] = {.type = NLA_U32 },
		[OVS_USERSPACE_ATTR_USERDATA] = {.type = NLA_UNSPEC },
		[OVS_USERSPACE_ATTR_EGRESS_TUN_PORT] = {.type = NLA_U32 },
	};
	struct nlattr *a[OVS_USERSPACE_ATTR_MAX + 1];
	int error;

	error = nla_parse_nested_deprecated(a, OVS_USERSPACE_ATTR_MAX, attr,
					    userspace_policy, NULL);
	if (error)
		return error;

	if (!a[OVS_USERSPACE_ATTR_PID] ||
	    !nla_get_u32(a[OVS_USERSPACE_ATTR_PID]))
		return -EINVAL;

	return 0;
}