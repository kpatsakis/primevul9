static curl_off_t VmsSpecialSize(const char * name,
                                 const struct_stat * stat_buf)
{
  switch(stat_buf->st_fab_rfm) {
  case FAB$C_VAR:
  case FAB$C_VFC:
    return vms_realfilesize(name, stat_buf);
    break;
  default:
    return stat_buf->st_size;
  }
}