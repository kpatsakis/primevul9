static u8 acl_to_byte(const sc_acl_entry_t *e)
{
	switch (e->method) {
	case SC_AC_NONE:
		return 0x00;
	case SC_AC_CHV:
		switch (e->key_ref) {
		case 1:
			return 0x01;
			break;
		case 2:
			return 0x02;
			break;
		default:
			return 0x00;
		}
		break;
	case SC_AC_TERM:
		return 0x04;
	case SC_AC_NEVER:
		return 0x0F;
	}
	return 0x00;
}