rdp_send_confirm_active(void)
{
	STREAM s;
	uint32 sec_flags = g_encryption ? (RDP5_FLAG | SEC_ENCRYPT) : RDP5_FLAG;
	uint16 caplen =
		RDP_CAPLEN_GENERAL + RDP_CAPLEN_BITMAP + RDP_CAPLEN_ORDER +
		RDP_CAPLEN_COLCACHE +
		RDP_CAPLEN_ACTIVATE + RDP_CAPLEN_CONTROL +
		RDP_CAPLEN_SHARE +
		RDP_CAPLEN_BRUSHCACHE + 0x58 + 0x08 + 0x08 + 0x34 /* unknown caps */  +
		4 /* w2k fix, sessionid */ ;

	if (g_rdp_version >= RDP_V5)
	{
		caplen += RDP_CAPLEN_BMPCACHE2;
		caplen += RDP_CAPLEN_NEWPOINTER;
	}
	else
	{
		caplen += RDP_CAPLEN_BMPCACHE;
		caplen += RDP_CAPLEN_POINTER;
	}

	s = sec_init(sec_flags, 6 + 14 + caplen + sizeof(RDP_SOURCE));

	out_uint16_le(s, 2 + 14 + caplen + sizeof(RDP_SOURCE));
	out_uint16_le(s, (RDP_PDU_CONFIRM_ACTIVE | 0x10));	/* Version 1 */
	out_uint16_le(s, (g_mcs_userid + 1001));

	out_uint32_le(s, g_rdp_shareid);
	out_uint16_le(s, 0x3ea);	/* userid */
	out_uint16_le(s, sizeof(RDP_SOURCE));
	out_uint16_le(s, caplen);

	out_uint8p(s, RDP_SOURCE, sizeof(RDP_SOURCE));
	out_uint16_le(s, 0xe);	/* num_caps */
	out_uint8s(s, 2);	/* pad */

	rdp_out_general_caps(s);
	rdp_out_bitmap_caps(s);
	rdp_out_order_caps(s);
	if (g_rdp_version >= RDP_V5)
	{
		rdp_out_bmpcache2_caps(s);
		rdp_out_newpointer_caps(s);
	}
	else
	{
		rdp_out_bmpcache_caps(s);
		rdp_out_pointer_caps(s);
	}
	rdp_out_colcache_caps(s);
	rdp_out_activate_caps(s);
	rdp_out_control_caps(s);
	rdp_out_share_caps(s);
	rdp_out_brushcache_caps(s);

	rdp_out_unknown_caps(s, 0x0d, 0x58, caps_0x0d);	/* CAPSTYPE_INPUT */
	rdp_out_unknown_caps(s, 0x0c, 0x08, caps_0x0c);	/* CAPSTYPE_SOUND */
	rdp_out_unknown_caps(s, 0x0e, 0x08, caps_0x0e);	/* CAPSTYPE_FONT */
	rdp_out_unknown_caps(s, 0x10, 0x34, caps_0x10);	/* CAPSTYPE_GLYPHCACHE */

	s_mark_end(s);
	sec_send(s, sec_flags);
}