PHP_FUNCTION(imagesetstyle)
{
	zval *IM, *styles;
	gdImagePtr im;
	int * stylearr;
	int index;
	HashPosition pos;
    int num_styles;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &IM, &styles) == FAILURE)  {
		return;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, &IM, -1, "Image", le_gd);

    num_styles = zend_hash_num_elements(HASH_OF(styles));
    if (num_styles == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "styles array must not be empty");
        RETURN_FALSE;
    }

	/* copy the style values in the stylearr */
	stylearr = safe_emalloc(sizeof(int), num_styles, 0);

	zend_hash_internal_pointer_reset_ex(HASH_OF(styles), &pos);

	for (index = 0;; zend_hash_move_forward_ex(HASH_OF(styles), &pos))	{
		zval ** item;

		if (zend_hash_get_current_data_ex(HASH_OF(styles), (void **) &item, &pos) == FAILURE) {
			break;
		}

		if (Z_TYPE_PP(item) != IS_LONG) {
			zval lval;
			lval = **item;
			zval_copy_ctor(&lval);
			convert_to_long(&lval);
			stylearr[index++] = Z_LVAL(lval);
		} else {
			stylearr[index++] = Z_LVAL_PP(item);
		}
	}

	gdImageSetStyle(im, stylearr, index);

	efree(stylearr);

	RETURN_TRUE;
}