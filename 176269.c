krb5_copy_ticket(krb5_context context,
		 const krb5_ticket *from,
		 krb5_ticket **to)
{
    krb5_error_code ret;
    krb5_ticket *tmp;

    *to = NULL;
    tmp = malloc(sizeof(*tmp));
    if (tmp == NULL)
	return krb5_enomem(context);
    if((ret = copy_EncTicketPart(&from->ticket, &tmp->ticket))){
	free(tmp);
	return ret;
    }
    ret = krb5_copy_principal(context, from->client, &tmp->client);
    if(ret){
	free_EncTicketPart(&tmp->ticket);
	free(tmp);
	return ret;
    }
    ret = krb5_copy_principal(context, from->server, &tmp->server);
    if(ret){
	krb5_free_principal(context, tmp->client);
	free_EncTicketPart(&tmp->ticket);
	free(tmp);
	return ret;
    }
    *to = tmp;
    return 0;
}