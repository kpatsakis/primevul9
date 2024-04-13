krb5_ticket_get_authorization_data_type(krb5_context context,
					krb5_ticket *ticket,
					int type,
					krb5_data *data)
{
    AuthorizationData *ad;
    krb5_error_code ret;
    krb5_boolean found = FALSE;

    krb5_data_zero(data);

    ad = ticket->ticket.authorization_data;
    if (ticket->ticket.authorization_data == NULL) {
	krb5_set_error_message(context, ENOENT,
			       N_("Ticket have not authorization data", ""));
	return ENOENT; /* XXX */
    }

    ret = find_type_in_ad(context, type, data, &found, TRUE,
			  &ticket->ticket.key, ad, 0);
    if (ret)
	return ret;
    if (!found) {
	krb5_set_error_message(context, ENOENT,
			       N_("Ticket have not "
				  "authorization data of type %d", ""),
			       type);
	return ENOENT; /* XXX */
    }
    return 0;
}