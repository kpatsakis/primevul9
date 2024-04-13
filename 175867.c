DEFUN (show_ip_bgp_vpnv4_rd_tags,
       show_ip_bgp_vpnv4_rd_tags_cmd,
       "show ip bgp vpnv4 rd ASN:nn_or_IP-address:nn tags",
       SHOW_STR
       IP_STR
       BGP_STR
       "Display VPNv4 NLRI specific information\n"
       "Display information for a route distinguisher\n"
       "VPN Route Distinguisher\n"
       "Display BGP tags for prefixes\n")
{
  int ret;
  struct prefix_rd prd;

  ret = str2prefix_rd (argv[0], &prd);
  if (! ret)
    {
      vty_out (vty, "%% Malformed Route Distinguisher%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  return bgp_show_mpls_vpn (vty, &prd, bgp_show_type_normal, NULL, 1);
}