bgp_mplsvpn_init (void)
{
  install_element (BGP_VPNV4_NODE, &vpnv4_network_cmd);
  install_element (BGP_VPNV4_NODE, &no_vpnv4_network_cmd);


  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_all_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_rd_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_all_tags_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_rd_tags_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_all_neighbor_routes_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_rd_neighbor_routes_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_all_neighbor_advertised_routes_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_rd_neighbor_advertised_routes_cmd);

  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_all_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_rd_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_all_tags_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_rd_tags_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_all_neighbor_routes_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_rd_neighbor_routes_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_all_neighbor_advertised_routes_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_rd_neighbor_advertised_routes_cmd);
}