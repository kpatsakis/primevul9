ffi_data_to_code_pointer (void *data)
{
  msegmentptr seg = segment_holding (gm, data);
  return add_segment_exec_offset (data, seg);
}