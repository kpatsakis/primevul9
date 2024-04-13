 */
void php_wddx_packet_start(wddx_packet *packet, char *comment, size_t comment_len)
{
	php_wddx_add_chunk_static(packet, WDDX_PACKET_S);
	if (comment) {
		php_wddx_add_chunk_static(packet, WDDX_HEADER_S);
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_S);
		php_wddx_add_chunk_ex(packet, comment, comment_len);
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_E);
		php_wddx_add_chunk_static(packet, WDDX_HEADER_E);
	} else {
		php_wddx_add_chunk_static(packet, WDDX_HEADER);
	}
	php_wddx_add_chunk_static(packet, WDDX_DATA_S);