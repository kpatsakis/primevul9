HandleMSLogonAuth(rfbClient *client)
{
  uint64_t gen, mod, resp, priv, pub, key;
  uint8_t username[256], password[64];
  rfbCredential *cred;

  if (!ReadFromRFBServer(client, (char *)&gen, 8)) return FALSE;
  if (!ReadFromRFBServer(client, (char *)&mod, 8)) return FALSE;
  if (!ReadFromRFBServer(client, (char *)&resp, 8)) return FALSE;
  gen = rfbClientSwap64IfLE(gen);
  mod = rfbClientSwap64IfLE(mod);
  resp = rfbClientSwap64IfLE(resp);

  if (!client->GetCredential)
  {
    rfbClientLog("GetCredential callback is not set.\n");
    return FALSE;
  }
  rfbClientLog("WARNING! MSLogon security type has very low password encryption! "\
    "Use it only with SSH tunnel or trusted network.\n");
  cred = client->GetCredential(client, rfbCredentialTypeUser);
  if (!cred)
  {
    rfbClientLog("Reading credential failed\n");
    return FALSE;
  }

  memset(username, 0, sizeof(username));
  strncpy((char *)username, cred->userCredential.username, sizeof(username)-1);
  memset(password, 0, sizeof(password));
  strncpy((char *)password, cred->userCredential.password, sizeof(password)-1);
  FreeUserCredential(cred);

  srand(time(NULL));
  priv = ((uint64_t)rand())<<32;
  priv |= (uint64_t)rand();

  pub = rfbPowM64(gen, priv, mod);
  key = rfbPowM64(resp, priv, mod);
  pub = rfbClientSwap64IfLE(pub);
  key = rfbClientSwap64IfLE(key);

  rfbClientEncryptBytes2(username, sizeof(username), (unsigned char *)&key);
  rfbClientEncryptBytes2(password, sizeof(password), (unsigned char *)&key);

  if (!WriteToRFBServer(client, (char *)&pub, 8)) return FALSE;
  if (!WriteToRFBServer(client, (char *)username, sizeof(username))) return FALSE;
  if (!WriteToRFBServer(client, (char *)password, sizeof(password))) return FALSE;

  /* Handle the SecurityResult message */
  if (!rfbHandleAuthResult(client)) return FALSE;

  return TRUE;
}