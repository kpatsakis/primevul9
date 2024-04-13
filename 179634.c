is_read_comp_null (record_parameters_st * record_params)
{
  if (record_params->compression_algorithm == GNUTLS_COMP_NULL)
    return 0;

  return 1;
}