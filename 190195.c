const char *ap_set_reqtail(cmd_parms *cmd, void *dummy, int arg)
{
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (err != NULL) {
        return err;
    }
    ap_mod_status_reqtail = arg;
    return NULL;
}