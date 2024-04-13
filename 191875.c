measured_bw_line_apply(measured_bw_line_t *parsed_line,
                       smartlist_t *routerstatuses)
{
  vote_routerstatus_t *rs = NULL;
  if (!routerstatuses)
    return 0;

  rs = smartlist_bsearch(routerstatuses, parsed_line->node_id,
                         compare_digest_to_vote_routerstatus_entry);

  if (rs) {
    rs->has_measured_bw = 1;
    rs->measured_bw_kb = (uint32_t)parsed_line->bw_kb;
  } else {
    log_info(LD_DIRSERV, "Node ID %s not found in routerstatus list",
             parsed_line->node_hex);
  }

  return rs != NULL;
}