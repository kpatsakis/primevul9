int copyonwrite_prepare(CLIENT* client)
{
	off_t i;
	if ((client->difffilename = malloc(1024))==NULL)
		err("Failed to allocate string for diff file name");
	snprintf(client->difffilename, 1024, "%s-%s-%d.diff",client->exportname,client->clientname,
		(int)getpid()) ;
	client->difffilename[1023]='\0';
	msg3(LOG_INFO,"About to create map and diff file %s",client->difffilename) ;
	client->difffile=open(client->difffilename,O_RDWR | O_CREAT | O_TRUNC,0600) ;
	if (client->difffile<0) err("Could not create diff file (%m)") ;
	if ((client->difmap=calloc(client->exportsize/DIFFPAGESIZE,sizeof(u32)))==NULL)
		err("Could not allocate memory") ;
	for (i=0;i<client->exportsize/DIFFPAGESIZE;i++) client->difmap[i]=(u32)-1 ;

	return 0;
}