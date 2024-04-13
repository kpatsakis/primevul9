static unsigned int map_operations (int commandbyte )
{
	unsigned int op = (unsigned int)-1;

	switch ( (commandbyte & 0xfe) ) {
		case 0xe2: /* append record */   op = SC_AC_OP_UPDATE; break;
		case 0x24: /* change password */ op = SC_AC_OP_UPDATE; break;
		case 0xe0: /* create */          op = SC_AC_OP_CREATE; break;
		case 0xe4: /* delete */          op = SC_AC_OP_DELETE; break;
		case 0xe8: /* exclude sfi */     op = SC_AC_OP_WRITE; break;
		case 0x82: /* external auth */   op = SC_AC_OP_READ; break;
		case 0xe6: /* include sfi */     op = SC_AC_OP_WRITE; break;
		case 0x88: /* internal auth */   op = SC_AC_OP_READ; break;
		case 0x04: /* invalidate */      op = SC_AC_OP_INVALIDATE; break;
		case 0x2a: /* perform sec. op */ op = SC_AC_OP_SELECT; break;
		case 0xb0: /* read binary */     op = SC_AC_OP_READ; break;
		case 0xb2: /* read record */     op = SC_AC_OP_READ; break;
		case 0x44: /* rehabilitate */    op = SC_AC_OP_REHABILITATE; break;
		case 0xa4: /* select */          op = SC_AC_OP_SELECT; break;
		case 0xee: /* set permanent */   op = SC_AC_OP_CREATE; break;
		case 0x2c: /* unblock password */op = SC_AC_OP_WRITE; break;
		case 0xd6: /* update binary */   op = SC_AC_OP_WRITE; break;
		case 0xdc: /* update record */   op = SC_AC_OP_WRITE; break;
		case 0x20: /* verify password */ op = SC_AC_OP_SELECT; break;
		case 0x60: /* admin group */     op = SC_AC_OP_CREATE; break;
	}
	return op;
}