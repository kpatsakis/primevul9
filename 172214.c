static OPJ_BOOL jpxSeek_callback(OPJ_OFF_T seek_pos, void * p_user_data)
{
  JPXData *jpxData = (JPXData *)p_user_data;

  if (seek_pos > jpxData->size)
    return OPJ_FALSE;
  jpxData->pos = seek_pos;
  return OPJ_TRUE;
}