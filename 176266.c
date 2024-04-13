krb5_ticket_get_endtime(krb5_context context,
			const krb5_ticket *ticket)
{
    return ticket->ticket.endtime;
}