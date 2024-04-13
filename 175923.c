static encodePtr find_encoder_by_type_name(sdlPtr sdl, const char *type)
{
	if (sdl && sdl->encoders) {
		HashPosition pos;
		encodePtr *enc;

		for (zend_hash_internal_pointer_reset_ex(sdl->encoders, &pos);
		     zend_hash_get_current_data_ex(sdl->encoders, (void **) &enc, &pos) == SUCCESS;
		     zend_hash_move_forward_ex(sdl->encoders, &pos)) {
		    if (strcmp((*enc)->details.type_str, type) == 0) {
				return *enc;
			}
		}
	}
	return NULL;
}