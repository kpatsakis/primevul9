static bool encrypt_des(const unsigned char *in, unsigned char *out,
                        const unsigned char *key_56)
{
  const CK_MECHANISM_TYPE mech = CKM_DES_ECB; /* DES cipher in ECB mode */
  PK11SlotInfo *slot = NULL;
  char key[8];                                /* expanded 64 bit key */
  SECItem key_item;
  PK11SymKey *symkey = NULL;
  SECItem *param = NULL;
  PK11Context *ctx = NULL;
  int out_len;                                /* not used, required by NSS */
  bool rv = FALSE;

  /* use internal slot for DES encryption (requires NSS to be initialized) */
  slot = PK11_GetInternalKeySlot();
  if(!slot)
    return FALSE;

  /* Expand the 56-bit key to 64-bits */
  extend_key_56_to_64(key_56, key);

  /* Set the key parity to odd */
  Curl_des_set_odd_parity((unsigned char *) key, sizeof(key));

  /* Import the key */
  key_item.data = (unsigned char *)key;
  key_item.len = sizeof(key);
  symkey = PK11_ImportSymKey(slot, mech, PK11_OriginUnwrap, CKA_ENCRYPT,
                             &key_item, NULL);
  if(!symkey)
    goto fail;

  /* Create the DES encryption context */
  param = PK11_ParamFromIV(mech, /* no IV in ECB mode */ NULL);
  if(!param)
    goto fail;
  ctx = PK11_CreateContextBySymKey(mech, CKA_ENCRYPT, symkey, param);
  if(!ctx)
    goto fail;

  /* Perform the encryption */
  if(SECSuccess == PK11_CipherOp(ctx, out, &out_len, /* outbuflen */ 8,
                                 (unsigned char *)in, /* inbuflen */ 8)
      && SECSuccess == PK11_Finalize(ctx))
    rv = /* all OK */ TRUE;

fail:
  /* cleanup */
  if(ctx)
    PK11_DestroyContext(ctx, PR_TRUE);
  if(symkey)
    PK11_FreeSymKey(symkey);
  if(param)
    SECITEM_FreeItem(param, PR_TRUE);
  PK11_FreeSlot(slot);
  return rv;
}