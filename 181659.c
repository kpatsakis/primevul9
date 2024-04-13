static bool encrypt_des(const unsigned char *in, unsigned char *out,
                        const unsigned char *key_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  struct {
    BLOBHEADER hdr;
    unsigned int len;
    char key[8];
  } blob;
  DWORD len = 8;

  /* Acquire the crypto provider */
  if(!CryptAcquireContext(&hprov, NULL, NULL, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT))
    return FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, sizeof(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.bVersion = 2;
  blob.hdr.aiKeyAlg = CALG_DES;
  blob.len = sizeof(blob.key);

  /* Expand the 56-bit key to 64-bits */
  extend_key_56_to_64(key_56, blob.key);

  /* Set the key parity to odd */
  Curl_des_set_odd_parity((unsigned char *) blob.key, sizeof(blob.key));

  /* Import the key */
  if(!CryptImportKey(hprov, (BYTE *) &blob, sizeof(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    return FALSE;
  }

  memcpy(out, in, 8);

  /* Perform the encryption */
  CryptEncrypt(hkey, 0, FALSE, 0, out, &len, len);

  CryptDestroyKey(hkey);
  CryptReleaseContext(hprov, 0);

  return TRUE;
}