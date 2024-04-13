timestamp_check (hive_h *h, hive_node_h node, int64_t timestamp)
{
  if (timestamp < 0) {
    SET_ERRNO (EINVAL,
               "negative time reported at %zu: %" PRIi64, node, timestamp);
    return -1;
  }

  return timestamp;
}