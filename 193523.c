soup_filter_input_stream_pollable_init (GPollableInputStreamInterface *pollable_interface,
					gpointer                       interface_data)
{
	pollable_interface->is_readable = soup_filter_input_stream_is_readable;
	pollable_interface->read_nonblocking = soup_filter_input_stream_read_nonblocking;
	pollable_interface->create_source = soup_filter_input_stream_create_source;
}