port_type_to_str (port_type type)
{
	switch (type) {
		case PT_NONE:		return "NONE";
		case PT_SCTP:		return "SCTP";
		case PT_TCP:		return "TCP";
		case PT_UDP:		return "UDP";
		case PT_DCCP:		return "DCCP";
		case PT_IPX:		return "IPX";
		case PT_DDP:		return "DDP";
		case PT_IDP:		return "IDP";
		case PT_USB:		return "USB";
		case PT_I2C:		return "I2C";
		case PT_IBQP:		return "IBQP";
		case PT_BLUETOOTH:	return "BLUETOOTH";
		default:		return "[Unknown]";
	}
}