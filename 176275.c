krb5_ticket_get_flags(krb5_context context,
		      const krb5_ticket *ticket)
{
    return TicketFlags2int(ticket->ticket.flags);
}