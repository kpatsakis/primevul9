ospf_header_dump (struct ospf_header *ospfh)
{
  char buf[9];
  u_int16_t auth_type = ntohs (ospfh->auth_type);

  zlog_debug ("Header");
  zlog_debug ("  Version %d", ospfh->version);
  zlog_debug ("  Type %d (%s)", ospfh->type,
	     LOOKUP (ospf_packet_type_str, ospfh->type));
  zlog_debug ("  Packet Len %d", ntohs (ospfh->length));
  zlog_debug ("  Router ID %s", inet_ntoa (ospfh->router_id));
  zlog_debug ("  Area ID %s", inet_ntoa (ospfh->area_id));
  zlog_debug ("  Checksum 0x%x", ntohs (ospfh->checksum));
  zlog_debug ("  AuType %s", LOOKUP (ospf_auth_type_str, auth_type));

  switch (auth_type)
    {
    case OSPF_AUTH_NULL:
      break;
    case OSPF_AUTH_SIMPLE:
      memset (buf, 0, 9);
      strncpy (buf, (char *) ospfh->u.auth_data, 8);
      zlog_debug ("  Simple Password %s", buf);
      break;
    case OSPF_AUTH_CRYPTOGRAPHIC:
      zlog_debug ("  Cryptographic Authentication");
      zlog_debug ("  Key ID %d", ospfh->u.crypt.key_id);
      zlog_debug ("  Auth Data Len %d", ospfh->u.crypt.auth_data_len);
      zlog_debug ("  Sequence number %ld",
		 (u_long)ntohl (ospfh->u.crypt.crypt_seqnum));
      break;
    default:
      zlog_debug ("* This is not supported authentication type");
      break;
    }
    
}