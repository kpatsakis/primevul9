mISDN_sock_create(struct net *net, struct socket *sock, int proto, int kern)
{
	int err = -EPROTONOSUPPORT;

	switch (proto) {
	case ISDN_P_BASE:
		err = base_sock_create(net, sock, proto, kern);
		break;
	case ISDN_P_TE_S0:
	case ISDN_P_NT_S0:
	case ISDN_P_TE_E1:
	case ISDN_P_NT_E1:
	case ISDN_P_LAPD_TE:
	case ISDN_P_LAPD_NT:
	case ISDN_P_B_RAW:
	case ISDN_P_B_HDLC:
	case ISDN_P_B_X75SLP:
	case ISDN_P_B_L2DTMF:
	case ISDN_P_B_L2DSP:
	case ISDN_P_B_L2DSPHDLC:
		err = data_sock_create(net, sock, proto, kern);
		break;
	default:
		return err;
	}

	return err;
}