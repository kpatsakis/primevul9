static enum enum_field_types mysql_to_perl_type(enum enum_field_types type)
{
  static enum enum_field_types enum_type;

  switch (type) {
  case MYSQL_TYPE_DOUBLE:
  case MYSQL_TYPE_FLOAT:
    enum_type= MYSQL_TYPE_DOUBLE;
    break;

  case MYSQL_TYPE_SHORT:
  case MYSQL_TYPE_TINY:
  case MYSQL_TYPE_LONG:
  case MYSQL_TYPE_INT24:
  case MYSQL_TYPE_YEAR:
#if IVSIZE >= 8
  case MYSQL_TYPE_LONGLONG:
    enum_type= MYSQL_TYPE_LONGLONG;
#else
    enum_type= MYSQL_TYPE_LONG;
#endif
    break;

#if MYSQL_VERSION_ID > NEW_DATATYPE_VERSION
  case MYSQL_TYPE_BIT:
    enum_type= MYSQL_TYPE_BIT;
    break;
#endif

#if MYSQL_VERSION_ID > NEW_DATATYPE_VERSION
  case MYSQL_TYPE_NEWDECIMAL:
#endif
  case MYSQL_TYPE_DECIMAL:
    enum_type= MYSQL_TYPE_DECIMAL;
    break;

#if IVSIZE < 8
  case MYSQL_TYPE_LONGLONG:
#endif
  case MYSQL_TYPE_DATE:
  case MYSQL_TYPE_TIME:
  case MYSQL_TYPE_DATETIME:
  case MYSQL_TYPE_NEWDATE:
  case MYSQL_TYPE_TIMESTAMP:
  case MYSQL_TYPE_VAR_STRING:
#if MYSQL_VERSION_ID > NEW_DATATYPE_VERSION
  case MYSQL_TYPE_VARCHAR:
#endif
  case MYSQL_TYPE_STRING:
    enum_type= MYSQL_TYPE_STRING;
    break;

#if MYSQL_VERSION_ID > GEO_DATATYPE_VERSION
  case MYSQL_TYPE_GEOMETRY:
#endif
  case MYSQL_TYPE_BLOB:
  case MYSQL_TYPE_TINY_BLOB:
    enum_type= MYSQL_TYPE_BLOB;
    break;

  default:
    enum_type= MYSQL_TYPE_STRING;    /* MySQL can handle all types as strings */
  }
  return(enum_type);
}