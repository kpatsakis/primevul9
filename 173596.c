SQLRETURN SQLSetDescFieldA( SQLHDESC descriptor_handle,
           SQLSMALLINT rec_number, 
           SQLSMALLINT field_identifier,
           SQLPOINTER value, 
           SQLINTEGER buffer_length )
{
    return SQLSetDescField( descriptor_handle,
           rec_number, 
           field_identifier,
           value, 
           buffer_length );
}