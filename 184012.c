smtp_message_printf(struct smtp_tx *tx, const char *fmt, ...)
{
	va_list	ap;
	int	len;

	if (tx->error)
		return -1;

	va_start(ap, fmt);
	len = vfprintf(tx->ofile, fmt, ap);
	va_end(ap);

	if (len == -1) {
		log_warn("smtp-in: session %016"PRIx64": vfprintf", tx->session->id);
		tx->error = TX_ERROR_IO;
	}
	else
		tx->odatalen += len;

	return len;
}