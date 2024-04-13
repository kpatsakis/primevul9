DEFUN (vpnv4_network,
       vpnv4_network_cmd,
       "network A.B.C.D/M rd ASN:nn_or_IP-address:nn tag WORD",
       "Specify a network to announce via BGP\n"
       "IP prefix <network>/<length>, e.g., 35.0.0.0/8\n"
       "Specify Route Distinguisher\n"
       "VPN Route Distinguisher\n"
       "BGP tag\n"
       "tag value\n")
{
  return bgp_static_set_vpnv4 (vty, argv[0], argv[1], argv[2]);
}