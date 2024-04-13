yang_read_message(struct lys_module *module,struct lys_restr *save,char *value, char *what, int message)
{
    int ret;

    if (message == ERROR_APP_TAG_KEYWORD) {
        ret = yang_check_string(module, &save->eapptag, "error_app_tag", what, value, NULL);
    } else {
        ret = yang_check_string(module, &save->emsg, "error_message", what, value, NULL);
    }
    return ret;
}