DEFUN (show_ip_bgp_vpnv4_all,
       show_ip_bgp_vpnv4_all_cmd,
       "show ip bgp vpnv4 all",
       SHOW_STR
       IP_STR
       BGP_STR
       "Display VPNv4 NLRI specific information\n"
       "Display information about all VPNv4 NLRIs\n")
{
  return bgp_show_mpls_vpn (vty, NULL, bgp_show_type_normal, NULL, 0);
}