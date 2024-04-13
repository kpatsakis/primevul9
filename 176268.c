krb5_ticket_get_client(krb5_context context,
		       const krb5_ticket *ticket,
		       krb5_principal *client)
{
    return krb5_copy_principal(context, ticket->client, client);
}