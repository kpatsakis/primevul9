_gnutls_encrypt (gnutls_session_t session, const uint8_t * headers,
                 size_t headers_size, const uint8_t * data,
                 size_t data_size, uint8_t * ciphertext,
                 size_t ciphertext_size, content_type_t type, 
                 record_parameters_st * params)
{
  gnutls_datum_t comp;
  int free_comp = 0;
  int ret;

  if (data_size == 0 || is_write_comp_null (params) == 0)
    {
      comp.data = (uint8_t*)data;
      comp.size = data_size;
    }
  else
    {
      /* Here comp is allocated and must be 
       * freed.
       */
      free_comp = 1;
      
      comp.size = ciphertext_size - headers_size;
      comp.data = gnutls_malloc(comp.size);
      if (comp.data == NULL)
        return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
      
      ret = _gnutls_compress(&params->write.compression_state, data, data_size, 
                             comp.data, comp.size, session->internals.priorities.stateless_compression);
      if (ret < 0)
        {
          gnutls_free(comp.data);
          return gnutls_assert_val(ret);
        }
      
      comp.size = ret;
    }

  ret = compressed_to_ciphertext (session, &ciphertext[headers_size],
                                       ciphertext_size - headers_size,
                                       &comp, type, params);

  if (free_comp)
    gnutls_free(comp.data);

  if (ret < 0)
    return gnutls_assert_val(ret);

  /* copy the headers */
  memcpy (ciphertext, headers, headers_size);
  
  if(IS_DTLS(session))
    _gnutls_write_uint16 (ret, &ciphertext[11]);
  else
    _gnutls_write_uint16 (ret, &ciphertext[3]);

  return ret + headers_size;
}