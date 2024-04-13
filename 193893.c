static bool on_option(char key, char *value) {

    switch (key) {
    case 'P':
        ctx.parent.auth_str = value;
        break;
    case 'p':
        ctx.key_auth_str = value;
        break;
    case 'G':
        ctx.key_type = tpm2_alg_util_from_optarg(value,
                tpm2_alg_util_flags_asymmetric | tpm2_alg_util_flags_symmetric);
        if (ctx.key_type == TPM2_ALG_ERROR) {
            LOG_ERR("Unsupported key type");
            return false;
        }
        return true;
    case 'i':
        ctx.input_key_file = value;
        break;
    case 'C':
        ctx.parent.ctx_path = value;
        break;
    case 'U':
        ctx.parent_key_public_file = value;
        break;
    case 'k':
        ctx.import_tpm = true;
        ctx.input_enc_key_file = value;
        break;
    case 'u':
        ctx.public_key_file = value;
        break;
    case 'r':
        ctx.private_key_file = value;
        break;
    case 'a':
        ctx.attrs = value;
        break;
    case 'g':
        ctx.name_alg = value;
        break;
    case 's':
        ctx.import_tpm = true;
        ctx.input_seed_file = value;
        break;
    case 'L':
        ctx.policy = value;
        break;
    case 0:
        ctx.auth_key_file = value;
        break;
    case 1:
        ctx.cp_hash_path = value;
        break;
    default:
        LOG_ERR("Invalid option");
        return false;
    }

    return true;
}