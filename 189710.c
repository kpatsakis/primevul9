const char * msc_alert_message(modsec_rec *msr, msre_actionset *actionset, const char *action_message,
    const char *rule_message)
{
    const char *message = NULL;

    if (rule_message == NULL) rule_message = "Unknown error.";

    if (action_message == NULL) {
        message = apr_psprintf(msr->mp, "%s%s",
            rule_message, msre_format_metadata(msr, actionset));
    }
    else {
        message = apr_psprintf(msr->mp, "%s %s%s", action_message,
            rule_message, msre_format_metadata(msr, actionset));
    }

    return message;
}