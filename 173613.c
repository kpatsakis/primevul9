SQLRETURN SQLSetDescField( SQLHDESC descriptor_handle,
           SQLSMALLINT rec_number, 
           SQLSMALLINT field_identifier,
           SQLPOINTER value, 
           SQLINTEGER buffer_length )
{
    /*
     * not quite sure how the descriptor can be
     * allocated to a statement, all the documentation talks
     * about state transitions on statement states, but the
     * descriptor may be allocated with more than one statement
     * at one time. Which one should I check ?
     */
    DMHDESC descriptor = (DMHDESC) descriptor_handle;
    SQLRETURN ret;
    SQLCHAR s1[ 100 + LOG_MESSAGE_LEN ];
    int isStrField = 0;

    /*
     * check descriptor
     */

    if ( !__validate_desc( descriptor ))
    {
        dm_log_write( __FILE__, 
                    __LINE__, 
                    LOG_INFO, 
                    LOG_INFO, 
                    "Error: SQL_INVALID_HANDLE" );

        return SQL_INVALID_HANDLE;
    }

    function_entry( descriptor );

    if ( log_info.log_flag )
    {
        sprintf( descriptor -> msg, "\n\t\tEntry:\
\n\t\t\tDescriptor = %p\
\n\t\t\tRec Number = %d\
\n\t\t\tField Ident = %s\
\n\t\t\tValue = %p\
\n\t\t\tBuffer Length = %d",
                descriptor,
                rec_number,
                __desc_attr_as_string( s1, field_identifier ),
                value, 
                (int)buffer_length );

        dm_log_write( __FILE__, 
                __LINE__, 
                LOG_INFO, 
                LOG_INFO, 
                descriptor -> msg );
    }

    thread_protect( SQL_HANDLE_DESC, descriptor );

    if ( descriptor -> connection -> state < STATE_C4 )
    {
        dm_log_write( __FILE__, 
                __LINE__, 
                LOG_INFO, 
                LOG_INFO, 
                "Error: HY010" );

        __post_internal_error( &descriptor -> error,
                ERROR_HY010, NULL,
                descriptor -> connection -> environment -> requested_version );

        return function_return_nodrv( SQL_HANDLE_DESC, descriptor, SQL_ERROR );
    }

    /*
     * check status of statements associated with this descriptor
     */

    if( __check_stmt_from_desc( descriptor, STATE_S8 ) ||
        __check_stmt_from_desc( descriptor, STATE_S9 ) ||
        __check_stmt_from_desc( descriptor, STATE_S10 ) ||
        __check_stmt_from_desc( descriptor, STATE_S11 ) ||
        __check_stmt_from_desc( descriptor, STATE_S12 ) ||
        __check_stmt_from_desc( descriptor, STATE_S13 ) ||
        __check_stmt_from_desc( descriptor, STATE_S14 ) ||
        __check_stmt_from_desc( descriptor, STATE_S15 )) {

        dm_log_write( __FILE__, 
                __LINE__, 
                LOG_INFO, 
                LOG_INFO, 
                "Error: HY010" );

        __post_internal_error( &descriptor -> error,
                ERROR_HY010, NULL,
                descriptor -> connection -> environment -> requested_version );

        return function_return_nodrv( SQL_HANDLE_DESC, descriptor, SQL_ERROR );
    }


    if ( rec_number < 0 )
    {
        __post_internal_error( &descriptor -> error,
                ERROR_07009, NULL,
                descriptor -> connection -> environment -> requested_version );

        return function_return_nodrv( SQL_HANDLE_DESC, descriptor, SQL_ERROR );
    }

    switch ( field_identifier )
    {
    /* Fixed-length fields: buffer_length is ignored */
    case SQL_DESC_ALLOC_TYPE:
    case SQL_DESC_ARRAY_SIZE:
    case SQL_DESC_ARRAY_STATUS_PTR:
    case SQL_DESC_BIND_OFFSET_PTR:
    case SQL_DESC_BIND_TYPE:
    case SQL_DESC_COUNT:
    case SQL_DESC_ROWS_PROCESSED_PTR:
    case SQL_DESC_AUTO_UNIQUE_VALUE:
    case SQL_DESC_CASE_SENSITIVE:
    case SQL_DESC_CONCISE_TYPE:
    case SQL_DESC_DATA_PTR:
    case SQL_DESC_DATETIME_INTERVAL_CODE:
    case SQL_DESC_DATETIME_INTERVAL_PRECISION:
    case SQL_DESC_DISPLAY_SIZE:
    case SQL_DESC_FIXED_PREC_SCALE:
    case SQL_DESC_INDICATOR_PTR:
    case SQL_DESC_LENGTH:
    case SQL_DESC_NULLABLE:
    case SQL_DESC_NUM_PREC_RADIX:
    case SQL_DESC_OCTET_LENGTH:
    case SQL_DESC_OCTET_LENGTH_PTR:
    case SQL_DESC_PARAMETER_TYPE:
    case SQL_DESC_PRECISION:
    case SQL_DESC_ROWVER:
    case SQL_DESC_SCALE:
    case SQL_DESC_SEARCHABLE:
    case SQL_DESC_TYPE:
    case SQL_DESC_UNNAMED:
    case SQL_DESC_UNSIGNED:
    case SQL_DESC_UPDATABLE:
        isStrField = 0;
        break;
    /* Pointer to data: buffer_length must be valid */
    case SQL_DESC_BASE_COLUMN_NAME:
    case SQL_DESC_BASE_TABLE_NAME:
    case SQL_DESC_CATALOG_NAME:
    case SQL_DESC_LABEL:
    case SQL_DESC_LITERAL_PREFIX:
    case SQL_DESC_LITERAL_SUFFIX:
    case SQL_DESC_LOCAL_TYPE_NAME:
    case SQL_DESC_NAME:
    case SQL_DESC_SCHEMA_NAME:
    case SQL_DESC_TABLE_NAME:
    case SQL_DESC_TYPE_NAME:
        isStrField = 1;
        break;
    default:
        isStrField = buffer_length != SQL_IS_POINTER && buffer_length != SQL_IS_INTEGER
            && buffer_length != SQL_IS_UINTEGER && buffer_length != SQL_IS_SMALLINT &&
            buffer_length != SQL_IS_USMALLINT;
    }
    
    if ( isStrField && buffer_length < 0 && buffer_length != SQL_NTS)
    {
        __post_internal_error( &descriptor -> error,
            ERROR_HY090, NULL,
            descriptor -> connection -> environment -> requested_version );

        return function_return_nodrv( SQL_HANDLE_DESC, descriptor, SQL_ERROR );
    }

    if ( field_identifier == SQL_DESC_COUNT && (intptr_t)value < 0 )
    {
        __post_internal_error( &descriptor -> error,
                ERROR_07009, NULL,
                descriptor -> connection -> environment -> requested_version );

        return function_return_nodrv( SQL_HANDLE_DESC, descriptor, SQL_ERROR );
    }
    
    if ( field_identifier == SQL_DESC_PARAMETER_TYPE && (intptr_t)value != SQL_PARAM_INPUT
        && (intptr_t)value != SQL_PARAM_OUTPUT && (intptr_t)value != SQL_PARAM_INPUT_OUTPUT &&
        (intptr_t)value != SQL_PARAM_INPUT_OUTPUT_STREAM && (intptr_t)value != SQL_PARAM_OUTPUT_STREAM )
    {
        __post_internal_error( &descriptor -> error,
                ERROR_HY105, NULL,
                descriptor -> connection -> environment -> requested_version );

        return function_return_nodrv( SQL_HANDLE_DESC, descriptor, SQL_ERROR );
    }

    if ( CHECK_SQLSETDESCFIELD( descriptor -> connection ))
    {
      ret = SQLSETDESCFIELD( descriptor -> connection,
              descriptor -> driver_desc,
              rec_number, 
              field_identifier,
              value, 
              buffer_length );
    }
    else if ( CHECK_SQLSETDESCFIELDW( descriptor -> connection ))
    {
      SQLWCHAR *s1 = NULL;

        if (isStrField)
      {
        s1 = ansi_to_unicode_alloc( value, buffer_length, descriptor -> connection, NULL );
            if (SQL_NTS != buffer_length)
            {
                buffer_length *= sizeof(SQLWCHAR);
            }
      }
      else
      {
              s1 = value;
      }
      ret = SQLSETDESCFIELDW( descriptor -> connection,
                descriptor -> driver_desc,
                rec_number, 
                field_identifier,
                s1, 
                buffer_length );
       
        if (isStrField)
       {
        if (s1)
          free(s1); 
       }
    }
    else 
	{
        dm_log_write( __FILE__, 
                __LINE__, 
                LOG_INFO, 
                LOG_INFO, 
                "Error: IM001" );

        __post_internal_error( &descriptor -> error,
                ERROR_IM001, NULL,
                descriptor -> connection -> environment -> requested_version );

        return function_return_nodrv( SQL_HANDLE_DESC, descriptor, SQL_ERROR );
    }

    if ( log_info.log_flag )
    {
        sprintf( descriptor -> msg, 
                "\n\t\tExit:[%s]",
                    __get_return_status( ret, s1 ));

        dm_log_write( __FILE__, 
                __LINE__, 
                LOG_INFO, 
                LOG_INFO, 
                descriptor -> msg );
    }

    return function_return( SQL_HANDLE_DESC, descriptor, ret );
}