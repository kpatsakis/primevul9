_free_duplicate_routerstatus_entry(void *e)
{
  log_warn(LD_DIR,
           "Network-status has two entries for the same router. "
           "Dropping one.");
  routerstatus_free(e);
}