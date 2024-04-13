receive_swallow_smtp(void)
{
/*XXX CHUNKING: not enough.  read chunks until RSET? */
if (message_ended >= END_NOTENDED)
  message_ended = read_message_data_smtp(NULL);
}