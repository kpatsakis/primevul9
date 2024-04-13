void gnutls_x509_crt_set_pin_function(gnutls_x509_crt_t crt,
				      gnutls_pin_callback_t fn,
				      void *userdata)
{
	crt->pin.cb = fn;
	crt->pin.data = userdata;
}