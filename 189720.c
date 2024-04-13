void msc_alert(modsec_rec *msr, int level, msre_actionset *actionset, const char *action_message,
    const char *rule_message)
{
    const char *message = msc_alert_message(msr, actionset, action_message, rule_message);

    msr_log(msr, level, "%s", message);
}