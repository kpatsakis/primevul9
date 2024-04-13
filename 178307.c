static int protocol_offset(int protocol)
{
	switch (protocol) {
	case IPPROTO_UDP:
		return 0;

	case IPPROTO_TCP:
		return 2;

	default:
		return -EINVAL;
	}

}