PHP_NAMED_FUNCTION(zif_locale_set_default)
{
	char* locale_name = NULL;
	int   len=0;

	if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC,  "s",
		&locale_name ,&len ) == FAILURE)
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 	"locale_set_default: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if(len == 0) {
		locale_name =  (char *)uloc_getDefault() ;
		len = strlen(locale_name);
	}

	zend_alter_ini_entry(LOCALE_INI_NAME, sizeof(LOCALE_INI_NAME), locale_name, len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);

	RETURN_TRUE;
}