static bool tpm2_tool_onstart(tpm2_options **opts) {

    const struct option topts[] = {
      { "parent-auth",        required_argument, NULL, 'P'},
      { "key-auth",           required_argument, NULL, 'p'},
      { "key-algorithm",      required_argument, NULL, 'G'},
      { "input",              required_argument, NULL, 'i'},
      { "parent-context",     required_argument, NULL, 'C'},
      { "parent-public",      required_argument, NULL, 'U'},
      { "private",            required_argument, NULL, 'r'},
      { "public",             required_argument, NULL, 'u'},
      { "attributes",  required_argument, NULL, 'a'},
      { "hash-algorithm",     required_argument, NULL, 'g'},
      { "seed",               required_argument, NULL, 's'},
      { "policy",             required_argument, NULL, 'L'},
      { "encryption-key",     required_argument, NULL, 'k'},
      { "passin",             required_argument, NULL,  0 },
      { "cphash",             required_argument, NULL,  1 },
    };

    *opts = tpm2_options_new("P:p:G:i:C:U:u:r:a:g:s:L:k:", ARRAY_LEN(topts),
            topts, on_option, NULL, 0);

    return *opts != NULL;
}