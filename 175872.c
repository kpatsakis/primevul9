show_adj_route_vpn (struct vty *vty, struct peer *peer, struct prefix_rd *prd)
{
  struct bgp *bgp;
  struct bgp_table *table;
  struct bgp_node *rn;
  struct bgp_node *rm;
  struct attr *attr;
  int rd_header;
  int header = 1;
  char v4_header[] = "   Network          Next Hop            Metric LocPrf Weight Path%s";

  bgp = bgp_get_default ();
  if (bgp == NULL)
    {
      vty_out (vty, "No BGP process is configured%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  for (rn = bgp_table_top (bgp->rib[AFI_IP][SAFI_MPLS_VPN]); rn;
       rn = bgp_route_next (rn))
    {
      if (prd && memcmp (rn->p.u.val, prd->val, 8) != 0)
        continue;

      if ((table = rn->info) != NULL)
        {
          rd_header = 1;

          for (rm = bgp_table_top (table); rm; rm = bgp_route_next (rm))
            if ((attr = rm->info) != NULL)
              {
                if (header)
                  {
                    vty_out (vty, "BGP table version is 0, local router ID is %s%s",
                             inet_ntoa (bgp->router_id), VTY_NEWLINE);
                    vty_out (vty, "Status codes: s suppressed, d damped, h history, * valid, > best, i - internal%s",
                             VTY_NEWLINE);
                    vty_out (vty, "Origin codes: i - IGP, e - EGP, ? - incomplete%s%s",
                             VTY_NEWLINE, VTY_NEWLINE);
                    vty_out (vty, v4_header, VTY_NEWLINE);
                    header = 0;
                  }

                if (rd_header)
                  {
                    u_int16_t type;
                    struct rd_as rd_as;
                    struct rd_ip rd_ip;
                    u_char *pnt;

                    pnt = rn->p.u.val;

                    /* Decode RD type. */
                    type = decode_rd_type (pnt);
                    /* Decode RD value. */
                    if (type == RD_TYPE_AS)
                      decode_rd_as (pnt + 2, &rd_as);
                    else if (type == RD_TYPE_IP)
                      decode_rd_ip (pnt + 2, &rd_ip);

                    vty_out (vty, "Route Distinguisher: ");

                    if (type == RD_TYPE_AS)
                      vty_out (vty, "%u:%d", rd_as.as, rd_as.val);
                    else if (type == RD_TYPE_IP)
                      vty_out (vty, "%s:%d", inet_ntoa (rd_ip.ip), rd_ip.val);

                    vty_out (vty, "%s", VTY_NEWLINE);
                    rd_header = 0;
                  }
                route_vty_out_tmp (vty, &rm->p, attr, SAFI_MPLS_VPN);
              }
        }
    }
  return CMD_SUCCESS;
}