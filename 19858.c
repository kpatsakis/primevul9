static bool actions_may_change_flow(const struct nlattr *actions)
{
	struct nlattr *nla;
	int rem;

	nla_for_each_nested(nla, actions, rem) {
		u16 action = nla_type(nla);

		switch (action) {
		case OVS_ACTION_ATTR_OUTPUT:
		case OVS_ACTION_ATTR_RECIRC:
		case OVS_ACTION_ATTR_TRUNC:
		case OVS_ACTION_ATTR_USERSPACE:
			break;

		case OVS_ACTION_ATTR_CT:
		case OVS_ACTION_ATTR_CT_CLEAR:
		case OVS_ACTION_ATTR_HASH:
		case OVS_ACTION_ATTR_POP_ETH:
		case OVS_ACTION_ATTR_POP_MPLS:
		case OVS_ACTION_ATTR_POP_NSH:
		case OVS_ACTION_ATTR_POP_VLAN:
		case OVS_ACTION_ATTR_PUSH_ETH:
		case OVS_ACTION_ATTR_PUSH_MPLS:
		case OVS_ACTION_ATTR_PUSH_NSH:
		case OVS_ACTION_ATTR_PUSH_VLAN:
		case OVS_ACTION_ATTR_SAMPLE:
		case OVS_ACTION_ATTR_SET:
		case OVS_ACTION_ATTR_SET_MASKED:
		case OVS_ACTION_ATTR_METER:
		case OVS_ACTION_ATTR_CHECK_PKT_LEN:
		case OVS_ACTION_ATTR_ADD_MPLS:
		case OVS_ACTION_ATTR_DEC_TTL:
		default:
			return true;
		}
	}
	return false;
}