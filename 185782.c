_gnutls_decrypt (gnutls_session_t session, 
                 gnutls_datum_t *ciphertext,
                 gnutls_datum_t *output, 
                 content_type_t type,
                 record_parameters_st * params, uint64 *sequence)
{
  int ret;

  if (ciphertext->size == 0)
    return 0;

  if (is_read_comp_null (params) == 0)
    {
      if (session->security_parameters.new_record_padding != 0)
        ret =
          ciphertext_to_compressed_new (session, ciphertext, output, 
                                        type, params, sequence);
      else
        ret =
          ciphertext_to_compressed (session, ciphertext, output,
                                    type, params, sequence);
      if (ret < 0)
        return gnutls_assert_val(ret);
      
      return ret;
    }
  else
    {
      gnutls_datum_t tmp;
      
      tmp.size = output->size;
      tmp.data = gnutls_malloc(tmp.size);
      if (tmp.data == NULL)
        return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

      if (session->security_parameters.new_record_padding != 0)
        ret =
          ciphertext_to_compressed_new (session, ciphertext, &tmp,
                                        type, params, sequence);
      else
        ret =
          ciphertext_to_compressed (session, ciphertext, &tmp, 
                                    type, params, sequence);
      if (ret < 0)
        goto leave;
      
      tmp.size = ret;
        
      if (ret != 0)
        {
          ret = _gnutls_decompress( &params->read.compression_state, 
                                    tmp.data, tmp.size, 
                                    output->data, output->size);
          if (ret < 0)
            goto leave;
        }
        
leave:
      gnutls_free(tmp.data);
      return ret;
    }
}