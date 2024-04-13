int sc_detect_card_presence(sc_reader_t *reader)
{
	int r;
	LOG_FUNC_CALLED(reader->ctx);
	if (reader->ops->detect_card_presence == NULL)
		LOG_FUNC_RETURN(reader->ctx, SC_ERROR_NOT_SUPPORTED);

	r = reader->ops->detect_card_presence(reader);
	LOG_FUNC_RETURN(reader->ctx, r);
}