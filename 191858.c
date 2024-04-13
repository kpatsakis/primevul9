dirserv_would_reject_router(const routerstatus_t *rs)
{
  uint32_t res;

  res = dirserv_get_status_impl(rs->identity_digest, rs->nickname,
                                rs->addr, rs->or_port,
                                NULL, NULL, LOG_DEBUG);

  return (res & FP_REJECT) != 0;
}