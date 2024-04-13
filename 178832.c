rfbBool rfbInitClient(rfbClient* client,int* argc,char** argv) {
  int i,j;

  if(argv && argc && *argc) {
    if(client->programName==0)
      client->programName=argv[0];

    for (i = 1; i < *argc; i++) {
      j = i;
      if (strcmp(argv[i], "-listen") == 0) {
	listenForIncomingConnections(client);
	break;
      } else if (strcmp(argv[i], "-listennofork") == 0) {
	listenForIncomingConnectionsNoFork(client, -1);
	break;
      } else if (strcmp(argv[i], "-play") == 0) {
	client->serverPort = -1;
	j++;
      } else if (i+1<*argc && strcmp(argv[i], "-encodings") == 0) {
	client->appData.encodingsString = argv[i+1];
	j+=2;
      } else if (i+1<*argc && strcmp(argv[i], "-compress") == 0) {
	client->appData.compressLevel = atoi(argv[i+1]);
	j+=2;
      } else if (i+1<*argc && strcmp(argv[i], "-quality") == 0) {
	client->appData.qualityLevel = atoi(argv[i+1]);
	j+=2;
      } else if (i+1<*argc && strcmp(argv[i], "-scale") == 0) {
        client->appData.scaleSetting = atoi(argv[i+1]);
        j+=2;
      } else if (i+1<*argc && strcmp(argv[i], "-qosdscp") == 0) {
        client->QoS_DSCP = atoi(argv[i+1]);
        j+=2;
      } else if (i+1<*argc && strcmp(argv[i], "-repeaterdest") == 0) {
	char* colon=strchr(argv[i+1],':');

	if(client->destHost)
	  free(client->destHost);
        client->destPort = 5900;

	client->destHost = strdup(argv[i+1]);
	if(client->destHost && colon) {
	  client->destHost[(int)(colon-argv[i+1])] = '\0';
	  client->destPort = atoi(colon+1);
	}
        j+=2;
      } else {
	char* colon=strrchr(argv[i],':');

	if(client->serverHost)
	  free(client->serverHost);

	if(colon) {
	  client->serverHost = strdup(argv[i]);
	  if(client->serverHost) {
	    client->serverHost[(int)(colon-argv[i])] = '\0';
	    client->serverPort = atoi(colon+1);
	  }
	} else {
	  client->serverHost = strdup(argv[i]);
	}
	if(client->serverPort >= 0 && client->serverPort < 5900)
	  client->serverPort += 5900;
      }
      /* purge arguments */
      if (j>i) {
	*argc-=j-i;
	memmove(argv+i,argv+j,(*argc-i)*sizeof(char*));
	i--;
      }
    }
  }

  if(!rfbInitConnection(client)) {
    rfbClientCleanup(client);
    return FALSE;
  }

  return TRUE;
}