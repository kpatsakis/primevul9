elg_generate (const gcry_sexp_t genparms, gcry_sexp_t *r_skey)
{
  gpg_err_code_t rc;
  unsigned int nbits;
  ELG_secret_key sk;
  gcry_mpi_t xvalue = NULL;
  gcry_sexp_t l1;
  gcry_mpi_t *factors = NULL;
  gcry_sexp_t misc_info = NULL;

  memset (&sk, 0, sizeof sk);

  rc = _gcry_pk_util_get_nbits (genparms, &nbits);
  if (rc)
    return rc;

  /* Parse the optional xvalue element. */
  l1 = sexp_find_token (genparms, "xvalue", 0);
  if (l1)
    {
      xvalue = sexp_nth_mpi (l1, 1, 0);
      sexp_release (l1);
      if (!xvalue)
        return GPG_ERR_BAD_MPI;
    }

  if (xvalue)
    {
      rc = generate_using_x (&sk, nbits, xvalue, &factors);
      mpi_free (xvalue);
    }
  else
    {
      rc = generate (&sk, nbits, &factors);
    }
  if (rc)
    goto leave;

  if (factors && factors[0])
    {
      int nfac;
      void **arg_list;
      char *buffer, *p;

      for (nfac = 0; factors[nfac]; nfac++)
        ;
      arg_list = xtrycalloc (nfac+1, sizeof *arg_list);
      if (!arg_list)
        {
          rc = gpg_err_code_from_syserror ();
          goto leave;
        }
      buffer = xtrymalloc (30 + nfac*2 + 2 + 1);
      if (!buffer)
        {
          rc = gpg_err_code_from_syserror ();
          xfree (arg_list);
          goto leave;
        }
      p = stpcpy (buffer, "(misc-key-info(pm1-factors");
      for(nfac = 0; factors[nfac]; nfac++)
        {
          p = stpcpy (p, "%m");
          arg_list[nfac] = factors + nfac;
        }
      p = stpcpy (p, "))");
      rc = sexp_build_array (&misc_info, NULL, buffer, arg_list);
      xfree (arg_list);
      xfree (buffer);
      if (rc)
        goto leave;
    }

  rc = sexp_build (r_skey, NULL,
                   "(key-data"
                   " (public-key"
                   "  (elg(p%m)(g%m)(y%m)))"
                   " (private-key"
                   "  (elg(p%m)(g%m)(y%m)(x%m)))"
                   " %S)",
                   sk.p, sk.g, sk.y,
                   sk.p, sk.g, sk.y, sk.x,
                   misc_info);

 leave:
  mpi_free (sk.p);
  mpi_free (sk.g);
  mpi_free (sk.y);
  mpi_free (sk.x);
  sexp_release (misc_info);
  if (factors)
    {
      gcry_mpi_t *mp;
      for (mp = factors; *mp; mp++)
        mpi_free (*mp);
      xfree (factors);
    }

  return rc;
}