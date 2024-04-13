krb5_ticket_get_server(krb5_context context,
		       const krb5_ticket *ticket,
		       krb5_principal *server)
{
    return krb5_copy_principal(context, ticket->server, server);
}