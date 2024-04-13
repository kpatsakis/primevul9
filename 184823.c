static bool frag_expire_skip_icmp(u32 user)
{
	return user == IP_DEFRAG_AF_PACKET ||
	       ip_defrag_user_in_between(user, IP_DEFRAG_CONNTRACK_IN,
					 __IP_DEFRAG_CONNTRACK_IN_END) ||
	       ip_defrag_user_in_between(user, IP_DEFRAG_CONNTRACK_BRIDGE_IN,
					 __IP_DEFRAG_CONNTRACK_BRIDGE_IN);
}