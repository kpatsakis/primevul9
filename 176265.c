int _gnutls_ecc_curve_fill_params(gnutls_ecc_curve_t curve, gnutls_pk_params_st* params)
{
const gnutls_ecc_curve_entry_st *st;
uint8_t val[MAX_ECC_CURVE_SIZE];
size_t val_size;
int ret;

  st = _gnutls_ecc_curve_get_params(curve);
  if (st == NULL)
    return gnutls_assert_val(GNUTLS_E_ECC_UNSUPPORTED_CURVE);

  val_size = sizeof(val);
  ret = _gnutls_hex2bin(st->prime, strlen(st->prime), val, &val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }

  ret = _gnutls_mpi_scan_nz(&params->params[ECC_PRIME], val, val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }
  params->params_nr++;
  
  val_size = sizeof(val);
  ret = _gnutls_hex2bin(st->order, strlen(st->order), val, &val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }

  ret = _gnutls_mpi_scan_nz(&params->params[ECC_ORDER], val, val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }
  params->params_nr++;
  
  val_size = sizeof(val);
  ret = _gnutls_hex2bin(st->A, strlen(st->A), val, &val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }

  ret = _gnutls_mpi_scan_nz(&params->params[ECC_A], val, val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }
  params->params_nr++;

  val_size = sizeof(val);
  ret = _gnutls_hex2bin(st->B, strlen(st->B), val, &val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }

  ret = _gnutls_mpi_scan_nz(&params->params[ECC_B], val, val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }
  params->params_nr++;
  
  val_size = sizeof(val);
  ret = _gnutls_hex2bin(st->Gx, strlen(st->Gx), val, &val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }

  ret = _gnutls_mpi_scan_nz(&params->params[ECC_GX], val, val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }
  params->params_nr++;
  
  val_size = sizeof(val);
  ret = _gnutls_hex2bin(st->Gy, strlen(st->Gy), val, &val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }

  ret = _gnutls_mpi_scan_nz(&params->params[ECC_GY], val, val_size);
  if (ret < 0)
    {
      gnutls_assert();
      goto cleanup;
    }
  params->params_nr++;
  
  return 0;

cleanup:
  gnutls_pk_params_release(params);
  return ret;

}