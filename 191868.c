dirserv_set_node_flags_from_authoritative_status(node_t *node,
                                                 uint32_t authstatus)
{
  node->is_valid = (authstatus & FP_INVALID) ? 0 : 1;
  node->is_bad_exit = (authstatus & FP_BADEXIT) ? 1 : 0;
}