MYSQL * STDCALL 
CLI_MYSQL_REAL_CONNECT(MYSQL *mysql,const char *host, const char *user,
		       const char *passwd, const char *db,
		       uint port, const char *unix_socket,ulong client_flag)
{
  char		buff[NAME_LEN+USERNAME_LENGTH+100];
  int           scramble_data_len, pkt_scramble_len= 0;
  char          *end,*host_info= 0, *server_version_end, *pkt_end;
  char          *scramble_data;
  const char    *scramble_plugin;
  ulong		pkt_length;
  NET		*net= &mysql->net;
#ifdef __WIN__
  HANDLE	hPipe=INVALID_HANDLE_VALUE;
#endif
#ifdef HAVE_SYS_UN_H
  struct	sockaddr_un UNIXaddr;
#endif
  DBUG_ENTER("mysql_real_connect");

  DBUG_PRINT("enter",("host: %s  db: %s  user: %s (client)",
		      host ? host : "(Null)",
		      db ? db : "(Null)",
		      user ? user : "(Null)"));

  /* Test whether we're already connected */
  if (net->vio)
  {
    set_mysql_error(mysql, CR_ALREADY_CONNECTED, unknown_sqlstate);
    DBUG_RETURN(0);
  }

  if (set_connect_attributes(mysql, buff, sizeof(buff)))
    DBUG_RETURN(0);

  mysql->methods= &client_methods;
  net->vio = 0;				/* If something goes wrong */
  mysql->client_flag=0;			/* For handshake */

  /* use default options */
  if (mysql->options.my_cnf_file || mysql->options.my_cnf_group)
  {
    mysql_read_default_options(&mysql->options,
			       (mysql->options.my_cnf_file ?
				mysql->options.my_cnf_file : "my"),
			       mysql->options.my_cnf_group);
    my_free(mysql->options.my_cnf_file);
    my_free(mysql->options.my_cnf_group);
    mysql->options.my_cnf_file=mysql->options.my_cnf_group=0;
  }

  /* Some empty-string-tests are done because of ODBC */
  if (!host || !host[0])
    host=mysql->options.host;
  if (!user || !user[0])
  {
    user=mysql->options.user;
    if (!user)
      user= "";
  }
  if (!passwd)
  {
    passwd=mysql->options.password;
#if !defined(DONT_USE_MYSQL_PWD) && !defined(MYSQL_SERVER)
    if (!passwd)
      passwd=getenv("MYSQL_PWD");		/* get it from environment */
#endif
    if (!passwd)
      passwd= "";
  }
  if (!db || !db[0])
    db=mysql->options.db;
  if (!port)
    port=mysql->options.port;
  if (!unix_socket)
    unix_socket=mysql->options.unix_socket;

  mysql->server_status=SERVER_STATUS_AUTOCOMMIT;
  DBUG_PRINT("info", ("Connecting"));

  /*
    Part 0: Grab a socket and connect it to the server
  */
#if defined(HAVE_SMEM)
  if ((!mysql->options.protocol ||
       mysql->options.protocol == MYSQL_PROTOCOL_MEMORY) &&
      (!host || !strcmp(host,LOCAL_HOST)))
  {
    HANDLE handle_map;
    DBUG_PRINT("info", ("Using shared memory"));

    handle_map= create_shared_memory(mysql, net,
                                     get_win32_connect_timeout(mysql));

    if (handle_map == INVALID_HANDLE_VALUE)
    {
      DBUG_PRINT("error",
		 ("host: '%s'  socket: '%s'  shared memory: %s  have_tcpip: %d",
		  host ? host : "<null>",
		  unix_socket ? unix_socket : "<null>",
		  (int) mysql->options.shared_memory_base_name,
		  (int) have_tcpip));
      if (mysql->options.protocol == MYSQL_PROTOCOL_MEMORY)
	goto error;

      /*
        Try also with PIPE or TCP/IP. Clear the error from
        create_shared_memory().
      */

      net_clear_error(net);
    }
    else
    {
      mysql->options.protocol=MYSQL_PROTOCOL_MEMORY;
      unix_socket = 0;
      host=mysql->options.shared_memory_base_name;
      my_snprintf(host_info=buff, sizeof(buff)-1,
                  ER(CR_SHARED_MEMORY_CONNECTION), host);
    }
  }
#endif /* HAVE_SMEM */
#if defined(HAVE_SYS_UN_H)
  if (!net->vio &&
      (!mysql->options.protocol ||
       mysql->options.protocol == MYSQL_PROTOCOL_SOCKET) &&
      (unix_socket || mysql_unix_port) &&
      (!host || !strcmp(host,LOCAL_HOST)))
  {
    my_socket sock= socket(AF_UNIX, SOCK_STREAM, 0);
    DBUG_PRINT("info", ("Using socket"));
    if (sock == SOCKET_ERROR)
    {
      set_mysql_extended_error(mysql, CR_SOCKET_CREATE_ERROR,
                               unknown_sqlstate,
                               ER(CR_SOCKET_CREATE_ERROR),
                               socket_errno);
      goto error;
    }

    net->vio= vio_new(sock, VIO_TYPE_SOCKET,
                      VIO_LOCALHOST | VIO_BUFFERED_READ);
    if (!net->vio)
    {
      DBUG_PRINT("error",("Unknow protocol %d ", mysql->options.protocol));
      set_mysql_error(mysql, CR_CONN_UNKNOW_PROTOCOL, unknown_sqlstate);
      closesocket(sock);
      goto error;
    }

    host= LOCAL_HOST;
    if (!unix_socket)
      unix_socket= mysql_unix_port;
    host_info= (char*) ER(CR_LOCALHOST_CONNECTION);
    DBUG_PRINT("info", ("Using UNIX sock '%s'", unix_socket));

    memset(&UNIXaddr, 0, sizeof(UNIXaddr));
    UNIXaddr.sun_family= AF_UNIX;
    strmake(UNIXaddr.sun_path, unix_socket, sizeof(UNIXaddr.sun_path)-1);

    if (vio_socket_connect(net->vio, (struct sockaddr *) &UNIXaddr,
                           sizeof(UNIXaddr), get_vio_connect_timeout(mysql)))
    {
      DBUG_PRINT("error",("Got error %d on connect to local server",
			  socket_errno));
      set_mysql_extended_error(mysql, CR_CONNECTION_ERROR,
                               unknown_sqlstate,
                               ER(CR_CONNECTION_ERROR),
                               unix_socket, socket_errno);
      vio_delete(net->vio);
      net->vio= 0;
      goto error;
    }
    mysql->options.protocol=MYSQL_PROTOCOL_SOCKET;
  }
#elif defined(_WIN32)
  if (!net->vio &&
      (mysql->options.protocol == MYSQL_PROTOCOL_PIPE ||
       (host && !strcmp(host,LOCAL_HOST_NAMEDPIPE)) ||
       (! have_tcpip && (unix_socket || !host && is_NT()))))
  {
    hPipe= create_named_pipe(mysql, get_win32_connect_timeout(mysql),
                             &host, &unix_socket);

    if (hPipe == INVALID_HANDLE_VALUE)
    {
      DBUG_PRINT("error",
		 ("host: '%s'  socket: '%s'  have_tcpip: %d",
		  host ? host : "<null>",
		  unix_socket ? unix_socket : "<null>",
		  (int) have_tcpip));
      if (mysql->options.protocol == MYSQL_PROTOCOL_PIPE ||
	  (host && !strcmp(host,LOCAL_HOST_NAMEDPIPE)) ||
	  (unix_socket && !strcmp(unix_socket,MYSQL_NAMEDPIPE)))
	goto error;
      /* Try also with TCP/IP */
    }
    else
    {
      net->vio= vio_new_win32pipe(hPipe);
      my_snprintf(host_info=buff, sizeof(buff)-1,
                  ER(CR_NAMEDPIPE_CONNECTION), unix_socket);
    }
  }
#endif
  DBUG_PRINT("info", ("net->vio: %p  protocol: %d",
                      net->vio, mysql->options.protocol));
  if (!net->vio &&
      (!mysql->options.protocol ||
       mysql->options.protocol == MYSQL_PROTOCOL_TCP))
  {
    struct addrinfo *res_lst, *client_bind_ai_lst= NULL, hints, *t_res;
    char port_buf[NI_MAXSERV];
    my_socket sock= SOCKET_ERROR;
    int gai_errno, saved_error= 0, status= -1, bind_result= 0;
    uint flags= VIO_BUFFERED_READ;

    unix_socket=0;				/* This is not used */

    if (!port)
      port= mysql_port;

    if (!host)
      host= LOCAL_HOST;

    my_snprintf(host_info=buff, sizeof(buff)-1, ER(CR_TCP_CONNECTION), host);
    DBUG_PRINT("info",("Server name: '%s'.  TCP sock: %d", host, port));

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype= SOCK_STREAM;
    hints.ai_protocol= IPPROTO_TCP;
    hints.ai_family= AF_UNSPEC;

    DBUG_PRINT("info",("IPV6 getaddrinfo %s", host));
    my_snprintf(port_buf, NI_MAXSERV, "%d", port);
    gai_errno= getaddrinfo(host, port_buf, &hints, &res_lst);

    if (gai_errno != 0) 
    { 
      /* 
        For DBUG we are keeping the right message but for client we default to
        historical error message.
      */
      DBUG_PRINT("info",("IPV6 getaddrinfo error %d", gai_errno));
      set_mysql_extended_error(mysql, CR_UNKNOWN_HOST, unknown_sqlstate,
                               ER(CR_UNKNOWN_HOST), host, errno);

      goto error;
    }

    /* Get address info for client bind name if it is provided */
    if (mysql->options.ci.bind_address)
    {
      int bind_gai_errno= 0;

      DBUG_PRINT("info",("Resolving addresses for client bind: '%s'",
                         mysql->options.ci.bind_address));
      /* Lookup address info for name */
      bind_gai_errno= getaddrinfo(mysql->options.ci.bind_address, 0,
                                  &hints, &client_bind_ai_lst);
      if (bind_gai_errno)
      {
        DBUG_PRINT("info",("client bind getaddrinfo error %d", bind_gai_errno));
        set_mysql_extended_error(mysql, CR_UNKNOWN_HOST, unknown_sqlstate,
                                 ER(CR_UNKNOWN_HOST),
                                 mysql->options.ci.bind_address,
                                 bind_gai_errno);

        freeaddrinfo(res_lst);
        goto error;
      }
      DBUG_PRINT("info", ("  got address info for client bind name"));
    }

    /*
      A hostname might map to multiple IP addresses (IPv4/IPv6). Go over the
      list of IP addresses until a successful connection can be established.
      For each IP address, attempt to bind the socket to each client address
      for the client-side bind hostname until the bind is successful.
    */
    DBUG_PRINT("info", ("Try connect on all addresses for host."));
    for (t_res= res_lst; t_res; t_res= t_res->ai_next)
    {
      DBUG_PRINT("info", ("Create socket, family: %d  type: %d  proto: %d",
                          t_res->ai_family, t_res->ai_socktype,
                          t_res->ai_protocol));

      sock= socket(t_res->ai_family, t_res->ai_socktype, t_res->ai_protocol);
      if (sock == SOCKET_ERROR)
      {
        DBUG_PRINT("info", ("Socket created was invalid"));
        /* Try next address if there is one */
        saved_error= socket_errno;
        continue;
      }

      if (client_bind_ai_lst)
      {
        struct addrinfo* curr_bind_ai= NULL;
        DBUG_PRINT("info", ("Attempting to bind socket to bind address(es)"));

        /*
           We'll attempt to bind to each of the addresses returned, until
           we find one that works.
           If none works, we'll try the next destination host address
           (if any)
        */
        curr_bind_ai= client_bind_ai_lst;

        while (curr_bind_ai != NULL)
        {
          /* Attempt to bind the socket to the given address */
          bind_result= bind(sock,
                            curr_bind_ai->ai_addr,
                            curr_bind_ai->ai_addrlen);
          if (!bind_result)
            break;   /* Success */

          DBUG_PRINT("info", ("bind failed, attempting another bind address"));
          /* Problem with the bind, move to next address if present */
          curr_bind_ai= curr_bind_ai->ai_next;
        }

        if (bind_result)
        {
          /*
            Could not bind to any client-side address with this destination
             Try the next destination address (if any)
          */
          DBUG_PRINT("info", ("All bind attempts with this address failed"));
          saved_error= socket_errno;
          closesocket(sock);
          continue;
        }
        DBUG_PRINT("info", ("Successfully bound client side of socket"));
      }

      /* Create a new Vio object to abstract the socket. */
      if (!net->vio)
      {
        if (!(net->vio= vio_new(sock, VIO_TYPE_TCPIP, flags)))
        {
          set_mysql_error(mysql, CR_OUT_OF_MEMORY, unknown_sqlstate);
          closesocket(sock);
          freeaddrinfo(res_lst);
          if (client_bind_ai_lst)
            freeaddrinfo(client_bind_ai_lst);
          goto error;
        }
      }
      /* Just reinitialize if one is already allocated. */
      else if (vio_reset(net->vio, VIO_TYPE_TCPIP, sock, NULL, flags))
      {
        set_mysql_error(mysql, CR_UNKNOWN_ERROR, unknown_sqlstate);
        closesocket(sock);
        freeaddrinfo(res_lst);
        if (client_bind_ai_lst)
          freeaddrinfo(client_bind_ai_lst);
        goto error;
      }

      DBUG_PRINT("info", ("Connect socket"));
      status= vio_socket_connect(net->vio, t_res->ai_addr, t_res->ai_addrlen,
                                 get_vio_connect_timeout(mysql));
      /*
        Here we rely on vio_socket_connect() to return success only if
        the connect attempt was really successful. Otherwise we would
        stop trying another address, believing we were successful.
      */
      if (!status)
        break;

      /*
        Save either the socket error status or the error code of
        the failed vio_connection operation. It is necessary to
        avoid having it overwritten by later operations.
      */
      saved_error= socket_errno;

      DBUG_PRINT("info", ("No success, close socket, try next address."));
      closesocket(sock);
    }
    DBUG_PRINT("info",
               ("End of connect attempts, sock: %d  status: %d  error: %d",
                sock, status, saved_error));

    freeaddrinfo(res_lst);
    if (client_bind_ai_lst)
      freeaddrinfo(client_bind_ai_lst);

    if (sock == SOCKET_ERROR)
    {
      set_mysql_extended_error(mysql, CR_IPSOCK_ERROR, unknown_sqlstate,
                                ER(CR_IPSOCK_ERROR), saved_error);
      goto error;
    }

    if (status)
    {
      DBUG_PRINT("error",("Got error %d on connect to '%s'", saved_error, host));
      set_mysql_extended_error(mysql, CR_CONN_HOST_ERROR, unknown_sqlstate,
                                ER(CR_CONN_HOST_ERROR), host, saved_error);
      goto error;
    }
  }

  DBUG_PRINT("info", ("net->vio: %p", net->vio));
  if (!net->vio)
  {
    DBUG_PRINT("error",("Unknow protocol %d ",mysql->options.protocol));
    set_mysql_error(mysql, CR_CONN_UNKNOW_PROTOCOL, unknown_sqlstate);
    goto error;
  }

  if (my_net_init(net, net->vio))
  {
    vio_delete(net->vio);
    net->vio = 0;
    set_mysql_error(mysql, CR_OUT_OF_MEMORY, unknown_sqlstate);
    goto error;
  }
  vio_keepalive(net->vio,TRUE);

  /* If user set read_timeout, let it override the default */
  if (mysql->options.read_timeout)
    my_net_set_read_timeout(net, mysql->options.read_timeout);

  /* If user set write_timeout, let it override the default */
  if (mysql->options.write_timeout)
    my_net_set_write_timeout(net, mysql->options.write_timeout);

  if (mysql->options.max_allowed_packet)
    net->max_packet_size= mysql->options.max_allowed_packet;

  /* Get version info */
  mysql->protocol_version= PROTOCOL_VERSION;	/* Assume this */
  if (mysql->options.connect_timeout &&
      (vio_io_wait(net->vio, VIO_IO_EVENT_READ,
                   get_vio_connect_timeout(mysql)) < 1))
  {
    set_mysql_extended_error(mysql, CR_SERVER_LOST, unknown_sqlstate,
                             ER(CR_SERVER_LOST_EXTENDED),
                             "waiting for initial communication packet",
                             socket_errno);
    goto error;
  }

  /*
    Part 1: Connection established, read and parse first packet
  */
  DBUG_PRINT("info", ("Read first packet."));

  if ((pkt_length=cli_safe_read(mysql)) == packet_error)
  {
    if (mysql->net.last_errno == CR_SERVER_LOST)
      set_mysql_extended_error(mysql, CR_SERVER_LOST, unknown_sqlstate,
                               ER(CR_SERVER_LOST_EXTENDED),
                               "reading initial communication packet",
                               socket_errno);
    goto error;
  }
  pkt_end= (char*)net->read_pos + pkt_length;
  /* Check if version of protocol matches current one */
  mysql->protocol_version= net->read_pos[0];
  DBUG_DUMP("packet",(uchar*) net->read_pos,10);
  DBUG_PRINT("info",("mysql protocol version %d, server=%d",
		     PROTOCOL_VERSION, mysql->protocol_version));
  if (mysql->protocol_version != PROTOCOL_VERSION)
  {
    set_mysql_extended_error(mysql, CR_VERSION_ERROR, unknown_sqlstate,
                             ER(CR_VERSION_ERROR), mysql->protocol_version,
                             PROTOCOL_VERSION);
    goto error;
  }
  server_version_end= end= strend((char*) net->read_pos+1);
  mysql->thread_id=uint4korr(end+1);
  end+=5;
  /* 
    Scramble is split into two parts because old clients do not understand
    long scrambles; here goes the first part.
  */
  scramble_data= end;
  scramble_data_len= SCRAMBLE_LENGTH_323 + 1;
  scramble_plugin= old_password_plugin_name;
  end+= scramble_data_len;

  if (pkt_end >= end + 1)
    mysql->server_capabilities=uint2korr(end);
  if (pkt_end >= end + 18)
  {
    /* New protocol with 16 bytes to describe server characteristics */
    mysql->server_language=end[2];
    mysql->server_status=uint2korr(end+3);
    mysql->server_capabilities|= uint2korr(end+5) << 16;
    pkt_scramble_len= end[7];
    if (pkt_scramble_len < 0)
    {
      set_mysql_error(mysql, CR_MALFORMED_PACKET,
                      unknown_sqlstate);        /* purecov: inspected */
      goto error;
    }
  }
  end+= 18;

  if (mysql->options.secure_auth && passwd[0] &&
      !(mysql->server_capabilities & CLIENT_SECURE_CONNECTION))
  {
    set_mysql_error(mysql, CR_SECURE_AUTH, unknown_sqlstate);
    goto error;
  }

  if (mysql_init_character_set(mysql))
    goto error;

  /* Save connection information */
  if (!my_multi_malloc(MYF(0),
		       &mysql->host_info, (uint) strlen(host_info)+1,
		       &mysql->host,      (uint) strlen(host)+1,
		       &mysql->unix_socket,unix_socket ?
		       (uint) strlen(unix_socket)+1 : (uint) 1,
		       &mysql->server_version,
		       (uint) (server_version_end - (char*) net->read_pos + 1),
		       NullS) ||
      !(mysql->user=my_strdup(user,MYF(0))) ||
      !(mysql->passwd=my_strdup(passwd,MYF(0))))
  {
    set_mysql_error(mysql, CR_OUT_OF_MEMORY, unknown_sqlstate);
    goto error;
  }
  strmov(mysql->host_info,host_info);
  strmov(mysql->host,host);
  if (unix_socket)
    strmov(mysql->unix_socket,unix_socket);
  else
    mysql->unix_socket=0;
  strmov(mysql->server_version,(char*) net->read_pos+1);
  mysql->port=port;

  if (pkt_end >= end + SCRAMBLE_LENGTH - SCRAMBLE_LENGTH_323 + 1)
  {
    /*
     move the first scramble part - directly in the NET buffer -
     to get a full continuous scramble. We've read all the header,
     and can overwrite it now.
    */
    memmove(end - SCRAMBLE_LENGTH_323, scramble_data,
            SCRAMBLE_LENGTH_323);
    scramble_data= end - SCRAMBLE_LENGTH_323;
    if (mysql->server_capabilities & CLIENT_PLUGIN_AUTH)
    {
      scramble_data_len= pkt_scramble_len;
      scramble_plugin= scramble_data + scramble_data_len;
      if (scramble_data + scramble_data_len > pkt_end)
        scramble_data_len= pkt_end - scramble_data;
    }
    else
    {
      scramble_data_len= pkt_end - scramble_data;
      scramble_plugin= native_password_plugin_name;
    }
  }
  else
    mysql->server_capabilities&= ~CLIENT_SECURE_CONNECTION;

  mysql->client_flag= client_flag;

  /*
    Part 2: invoke the plugin to send the authentication data to the server
  */

  if (run_plugin_auth(mysql, scramble_data, scramble_data_len,
                      scramble_plugin, db))
    goto error;

  /*
    Part 3: authenticated, finish the initialization of the connection
  */

  if (mysql->client_flag & CLIENT_COMPRESS)      /* We will use compression */
    net->compress=1;

#ifdef CHECK_LICENSE 
  if (check_license(mysql))
    goto error;
#endif

  if (db && !mysql->db && mysql_select_db(mysql, db))
  {
    if (mysql->net.last_errno == CR_SERVER_LOST)
        set_mysql_extended_error(mysql, CR_SERVER_LOST, unknown_sqlstate,
                                 ER(CR_SERVER_LOST_EXTENDED),
                                 "Setting intital database",
                                 errno);
    goto error;
  }

  /*
     Using init_commands is not supported when connecting from within the
     server.
  */
#ifndef MYSQL_SERVER
  if (mysql->options.init_commands)
  {
    DYNAMIC_ARRAY *init_commands= mysql->options.init_commands;
    char **ptr= (char**)init_commands->buffer;
    char **end_command= ptr + init_commands->elements;

    my_bool reconnect=mysql->reconnect;
    mysql->reconnect=0;

    for (; ptr < end_command; ptr++)
    {
      int status;

      if (mysql_real_query(mysql,*ptr, (ulong) strlen(*ptr)))
	goto error;

      do {
        if (mysql->fields)
        {
          MYSQL_RES *res;
          if (!(res= cli_use_result(mysql)))
            goto error;
          mysql_free_result(res);
        }
        if ((status= mysql_next_result(mysql)) > 0)
          goto error;
      } while (status == 0);
    }
    mysql->reconnect=reconnect;
  }
#endif

  DBUG_PRINT("exit", ("Mysql handler: 0x%lx", (long) mysql));
  DBUG_RETURN(mysql);

error:
  DBUG_PRINT("error",("message: %u/%s (%s)",
                      net->last_errno,
                      net->sqlstate,
                      net->last_error));
  {
    /* Free alloced memory */
    end_server(mysql);
    mysql_close_free(mysql);
    if (!(client_flag & CLIENT_REMEMBER_OPTIONS))
      mysql_close_free_options(mysql);
  }
  DBUG_RETURN(0);