static sdlTypePtr model_array_element(sdlContentModelPtr model)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT: {
			if (model->max_occurs == -1 || model->max_occurs > 1) {
			  return model->u.element;
			} else {
			  return NULL;
			}
		}
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr *tmp;
			HashPosition pos;

			if (zend_hash_num_elements(model->u.content) != 1) {
			  return NULL;
			}
			zend_hash_internal_pointer_reset_ex(model->u.content, &pos);
			zend_hash_get_current_data_ex(model->u.content, (void**)&tmp, &pos);
			return model_array_element(*tmp);
		}
		case XSD_CONTENT_GROUP: {
			return model_array_element(model->u.group->model);
		}
		default:
		  break;
	}
	return NULL;
}