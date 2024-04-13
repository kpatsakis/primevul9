krb5_free_ticket(krb5_context context,
		 krb5_ticket *ticket)
{
    free_EncTicketPart(&ticket->ticket);
    krb5_free_principal(context, ticket->client);
    krb5_free_principal(context, ticket->server);
    free(ticket);
    return 0;
}