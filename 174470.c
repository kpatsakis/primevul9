rdp_out_general_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_GENERAL);
	out_uint16_le(s, RDP_CAPLEN_GENERAL);

	out_uint16_le(s, 1);	/* OS major type */
	out_uint16_le(s, 3);	/* OS minor type */
	out_uint16_le(s, 0x200);	/* Protocol version */
	out_uint16(s, 0);	/* Pad */
	out_uint16(s, 0);	/* Compression types */
	out_uint16_le(s, (g_rdp_version >= RDP_V5) ? 0x40d : 0);
	/* Pad, according to T.128. 0x40d seems to 
	   trigger
	   the server to start sending RDP5 packets. 
	   However, the value is 0x1d04 with W2KTSK and
	   NT4MS. Hmm.. Anyway, thankyou, Microsoft,
	   for sending such information in a padding 
	   field.. */
	out_uint16(s, 0);	/* Update capability */
	out_uint16(s, 0);	/* Remote unshare capability */
	out_uint16(s, 0);	/* Compression level */
	out_uint16(s, 0);	/* Pad */
}