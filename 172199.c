static OPJ_SIZE_T jpxRead_callback(void * p_buffer, OPJ_SIZE_T p_nb_bytes, void * p_user_data)
{
  JPXData *jpxData = (JPXData *)p_user_data;
  int len;

  len = jpxData->size - jpxData->pos;
  if (len < 0)
    len = 0;
  if (len == 0)
    return (OPJ_SIZE_T)-1;  /* End of file! */
  if ((OPJ_SIZE_T)len > p_nb_bytes)
    len = p_nb_bytes;
  memcpy(p_buffer, jpxData->data + jpxData->pos, len);
  jpxData->pos += len;
  return len;
}