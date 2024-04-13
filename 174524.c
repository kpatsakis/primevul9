email_write_headers(FILE *stream,
		    const char *FromAddress,
		    const char *FinalSubject,
		    const char *Addresses,
		    int NumAddresses)
{
	if (FromAddress) {
		fputs("From: ", stream);
		email_write_header_string(stream, FromAddress);
		fputc('\n', stream);
	}
	fputs("Subject: ", stream);
	email_write_header_string(stream, FinalSubject);
	fputc('\n', stream);

	fputs("To: ", stream);
	for (int i = 0; i < NumAddresses; ++i) {
		if (i > 0) {
			fputs(", ", stream);
		}
		while (*Addresses == '\0') {
			Addresses++;
		}
		email_write_header_string(stream, Addresses);
		Addresses += strlen(Addresses) + 1;
	}
	fputs("\n\n", stream);
}