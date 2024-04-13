CURLcode  Curl_ntlm_core_mk_lmv2_resp(unsigned char *ntlmv2hash,
                                      unsigned char *challenge_client,
                                      unsigned char *challenge_server,
                                      unsigned char *lmresp)
{
  unsigned char data[16];
  unsigned char hmac_output[16];
  CURLcode result = CURLE_OK;

  memcpy(&data[0], challenge_server, 8);
  memcpy(&data[8], challenge_client, 8);

  result = Curl_hmac_md5(ntlmv2hash, 16, &data[0], 16, hmac_output);
  if(result)
    return result;

  /* Concatenate the HMAC MD5 output  with the client nonce */
  memcpy(lmresp, hmac_output, 16);
  memcpy(lmresp + 16, challenge_client, 8);

  return result;
}