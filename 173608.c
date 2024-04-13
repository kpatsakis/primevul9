int main( int argc, char *argv[] )
{
    int     nArg, count;
    int     bHTMLTable                  = 0;
    int     bBatch                      = 0;
    int     cDelimiter                  = 0;
    int     bColumnNames                = 0;
    char    *szDSN;
    char    *szUID;
    char    *szPWD;
    char    *szSQL;
    char    *pEscapeChar;
    int     buffer_size = 9000;

    szDSN = NULL;
    szUID = NULL;
    szPWD = NULL;

    if ( argc < 2 )
    {
        fputs( szSyntax, stderr );
        exit( 1 );
    }

#ifdef HAVE_SETLOCALE
    /*
     * Default locale
     */
    setlocale( LC_ALL, "" );
#endif

    /****************************
     * PARSE ARGS
     ***************************/
    for ( nArg = 1, count = 1 ; nArg < argc; nArg++ )
    {
        if ( argv[nArg][0] == '-' )
        {
            /* Options */
            switch ( argv[nArg][1] )
            {
                case 'd':
                    cDelimiter = argv[nArg][2];
                    break;
                case 's':
                    buffer_size = atoi( &(argv[nArg][2]) );
                    break;
                case 'w':
                    bHTMLTable = 1;
                    break;
                case 'b':
                    bBatch = 1;
                    break;
                case 'c':
                    bColumnNames = 1;
                    break;
                case 'v':
                    bVerbose = 1;
                    break;
                case 'e':
                    buseED = 1;
                    break;
                case '-':
                    printf( "unixODBC " VERSION "\n" );
                    exit(0);
#ifdef HAVE_STRTOL
                case 'x':
                    cDelimiter = strtol( argv[nArg]+2, NULL, 0 );
                    break;
#endif
#ifdef HAVE_SETLOCALE
                case 'l':
                    if ( !setlocale( LC_ALL, argv[nArg]+2 ))
                    {
                        fprintf( stderr, "isql: can't set locale to '%s'\n", argv[nArg]+2 );
                        exit ( -1 );
                    }
                    break;
#endif
                default:
                    fputs( szSyntax, stderr );
                    exit( 1 );
            }
            continue;
        }
        else if ( count == 1 )
            szDSN = argv[nArg];
        else if ( count == 2 )
            szUID = argv[nArg];
        else if ( count == 3 )
            szPWD = argv[nArg];
        count++;
    }

    szSQL = calloc( 1, buffer_size + 1 );

    /****************************
     * CONNECT
     ***************************/
    if ( !OpenDatabase( &hEnv, &hDbc, szDSN, szUID, szPWD ) )
        exit( 1 );

    /****************************
     * EXECUTE
     ***************************/
    if ( !bBatch )
    {
        printf( "+---------------------------------------+\n" );
        printf( "| Connected!                            |\n" );
        printf( "|                                       |\n" );
        printf( "| sql-statement                         |\n" );
        printf( "| help [tablename]                      |\n" );
        printf( "| quit                                  |\n" );
        printf( "|                                       |\n" );
        printf( "+---------------------------------------+\n" );
    }
    do
    {
        if ( !bBatch )
#ifndef HAVE_READLINE
            printf( "SQL> " );
#else
        {
            char *line;
            int malloced;

            line=readline("SQL> ");
            if ( !line )        /* EOF - ctrl D */
            {
                malloced = 1;
                line = strdup( "quit" );
            }
            else
            {
                malloced = 0;
            }
            strncpy(szSQL, line, buffer_size );
            add_history(line);
            if ( malloced )
            {
                free(line);
            }
        }
        else
#endif
        {
            char *line;
            int malloced;

            line = fgets( szSQL, buffer_size, stdin );
            if ( !line )        /* EOF - ctrl D */
            {
                malloced = 1;
                line = strdup( "quit" );
            }
            else
            {
                malloced = 0;
            }
            strncpy(szSQL, line, buffer_size );
            if ( malloced )
            {
                free(line);
            }
        }

        /* strip away escape chars */
        while ( (pEscapeChar=(char*)strchr(szSQL, '\n')) != NULL || (pEscapeChar=(char*)strchr(szSQL, '\r')) != NULL )
            *pEscapeChar = ' ';

        if ( szSQL[1] != '\0' )
        {
            if ( strncmp( szSQL, "quit", 4 ) == 0 )
                szSQL[1] = '\0';
            else if ( strncmp( szSQL, "help", 4 ) == 0 )
                ExecuteHelp( hDbc, szSQL, cDelimiter, bColumnNames, bHTMLTable );
            else if (memcmp(szSQL, "--", 2) != 0)
                ExecuteSQL( hDbc, szSQL, cDelimiter, bColumnNames, bHTMLTable );
        }

    } while ( szSQL[1] != '\0' );

    /****************************
     * DISCONNECT
     ***************************/
    CloseDatabase( hEnv, hDbc );

    exit( 0 );
}