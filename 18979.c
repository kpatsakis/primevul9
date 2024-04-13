verify_flags (krb5_context context,
	      krb5_kdc_configuration *config,
	      const EncTicketPart *et,
	      const char *pstr)
{
    if(et->endtime < kdc_time){
	kdc_log(context, config, 4, "Ticket expired (%s)", pstr);
	return KRB5KRB_AP_ERR_TKT_EXPIRED;
    }
    if(et->flags.invalid){
	kdc_log(context, config, 4, "Ticket not valid (%s)", pstr);
	return KRB5KRB_AP_ERR_TKT_NYV;
    }
    return 0;
}