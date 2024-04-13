static OPJ_OFF_T jpxSkip_callback(OPJ_OFF_T skip, void * p_user_data)
{
  JPXData *jpxData = (JPXData *)p_user_data;

  jpxData->pos += (skip > jpxData->size - jpxData->pos) ? jpxData->size - jpxData->pos : skip;
  /* Always return input value to avoid "Problem with skipping JPEG2000 box, stream error" */
  return skip;
}