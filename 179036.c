
static void date_register_classes(TSRMLS_D)
{
	zend_class_entry ce_date, ce_immutable, ce_timezone, ce_interval, ce_period, ce_interface;

	INIT_CLASS_ENTRY(ce_interface, "DateTimeInterface", date_funcs_interface);
	date_ce_interface = zend_register_internal_interface(&ce_interface TSRMLS_CC);
	date_ce_interface->interface_gets_implemented = implement_date_interface_handler;

	INIT_CLASS_ENTRY(ce_date, "DateTime", date_funcs_date);
	ce_date.create_object = date_object_new_date;
	date_ce_date = zend_register_internal_class_ex(&ce_date, NULL, NULL TSRMLS_CC);
	memcpy(&date_object_handlers_date, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_date.clone_obj = date_object_clone_date;
	date_object_handlers_date.compare_objects = date_object_compare_date;
	date_object_handlers_date.get_properties = date_object_get_properties;
	date_object_handlers_date.get_gc = date_object_get_gc;
	zend_class_implements(date_ce_date TSRMLS_CC, 1, date_ce_interface);

#define REGISTER_DATE_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_stringl(date_ce_date, const_name, sizeof(const_name)-1, value, sizeof(value)-1 TSRMLS_CC);

	REGISTER_DATE_CLASS_CONST_STRING("ATOM",    DATE_FORMAT_RFC3339);
	REGISTER_DATE_CLASS_CONST_STRING("COOKIE",  DATE_FORMAT_COOKIE);
	REGISTER_DATE_CLASS_CONST_STRING("ISO8601", DATE_FORMAT_ISO8601);
	REGISTER_DATE_CLASS_CONST_STRING("RFC822",  DATE_FORMAT_RFC822);
	REGISTER_DATE_CLASS_CONST_STRING("RFC850",  DATE_FORMAT_RFC850);
	REGISTER_DATE_CLASS_CONST_STRING("RFC1036", DATE_FORMAT_RFC1036);
	REGISTER_DATE_CLASS_CONST_STRING("RFC1123", DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("RFC2822", DATE_FORMAT_RFC2822);
	REGISTER_DATE_CLASS_CONST_STRING("RFC3339", DATE_FORMAT_RFC3339);
	REGISTER_DATE_CLASS_CONST_STRING("RSS",     DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("W3C",     DATE_FORMAT_RFC3339);

	INIT_CLASS_ENTRY(ce_immutable, "DateTimeImmutable", date_funcs_immutable);
	ce_immutable.create_object = date_object_new_date;
	date_ce_immutable = zend_register_internal_class_ex(&ce_immutable, NULL, NULL TSRMLS_CC);
	memcpy(&date_object_handlers_immutable, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_immutable.clone_obj = date_object_clone_date;
	date_object_handlers_immutable.compare_objects = date_object_compare_date;
	date_object_handlers_immutable.get_properties = date_object_get_properties;
	zend_class_implements(date_ce_immutable TSRMLS_CC, 1, date_ce_interface);

	INIT_CLASS_ENTRY(ce_timezone, "DateTimeZone", date_funcs_timezone);
	ce_timezone.create_object = date_object_new_timezone;
	date_ce_timezone = zend_register_internal_class_ex(&ce_timezone, NULL, NULL TSRMLS_CC);
	memcpy(&date_object_handlers_timezone, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_timezone.clone_obj = date_object_clone_timezone;
	date_object_handlers_timezone.get_properties = date_object_get_properties_timezone;
	date_object_handlers_timezone.get_gc = date_object_get_gc_timezone;

#define REGISTER_TIMEZONE_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_long(date_ce_timezone, const_name, sizeof(const_name)-1, value TSRMLS_CC);

	REGISTER_TIMEZONE_CLASS_CONST_STRING("AFRICA",      PHP_DATE_TIMEZONE_GROUP_AFRICA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("AMERICA",     PHP_DATE_TIMEZONE_GROUP_AMERICA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ANTARCTICA",  PHP_DATE_TIMEZONE_GROUP_ANTARCTICA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ARCTIC",      PHP_DATE_TIMEZONE_GROUP_ARCTIC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ASIA",        PHP_DATE_TIMEZONE_GROUP_ASIA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ATLANTIC",    PHP_DATE_TIMEZONE_GROUP_ATLANTIC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("AUSTRALIA",   PHP_DATE_TIMEZONE_GROUP_AUSTRALIA);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("EUROPE",      PHP_DATE_TIMEZONE_GROUP_EUROPE);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("INDIAN",      PHP_DATE_TIMEZONE_GROUP_INDIAN);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("PACIFIC",     PHP_DATE_TIMEZONE_GROUP_PACIFIC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("UTC",         PHP_DATE_TIMEZONE_GROUP_UTC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ALL",         PHP_DATE_TIMEZONE_GROUP_ALL);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("ALL_WITH_BC", PHP_DATE_TIMEZONE_GROUP_ALL_W_BC);
	REGISTER_TIMEZONE_CLASS_CONST_STRING("PER_COUNTRY", PHP_DATE_TIMEZONE_PER_COUNTRY);

	INIT_CLASS_ENTRY(ce_interval, "DateInterval", date_funcs_interval);
	ce_interval.create_object = date_object_new_interval;
	date_ce_interval = zend_register_internal_class_ex(&ce_interval, NULL, NULL TSRMLS_CC);
	memcpy(&date_object_handlers_interval, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_interval.clone_obj = date_object_clone_interval;
	date_object_handlers_interval.read_property = date_interval_read_property;
	date_object_handlers_interval.write_property = date_interval_write_property;
	date_object_handlers_interval.get_properties = date_object_get_properties_interval;
	date_object_handlers_interval.get_property_ptr_ptr = NULL;
	date_object_handlers_interval.get_gc = date_object_get_gc_interval;

	INIT_CLASS_ENTRY(ce_period, "DatePeriod", date_funcs_period);
	ce_period.create_object = date_object_new_period;
	date_ce_period = zend_register_internal_class_ex(&ce_period, NULL, NULL TSRMLS_CC);
	date_ce_period->get_iterator = date_object_period_get_iterator;
	date_ce_period->iterator_funcs.funcs = &date_period_it_funcs;
	zend_class_implements(date_ce_period TSRMLS_CC, 1, zend_ce_traversable);
	memcpy(&date_object_handlers_period, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_period.clone_obj = date_object_clone_period;
	date_object_handlers_period.get_properties = date_object_get_properties_period;
	date_object_handlers_period.get_property_ptr_ptr = NULL;
	date_object_handlers_period.get_gc = date_object_get_gc_period;
	date_object_handlers_period.read_property = date_period_read_property;
	date_object_handlers_period.write_property = date_period_write_property;

#define REGISTER_PERIOD_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_long(date_ce_period, const_name, sizeof(const_name)-1, value TSRMLS_CC);

	REGISTER_PERIOD_CLASS_CONST_STRING("EXCLUDE_START_DATE", PHP_DATE_PERIOD_EXCLUDE_START_DATE);