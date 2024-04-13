void SSH_Access::LogSSHMessage()
{
   const char *b;
   int s;
   pty_recv_buf->Get(&b,&s);
   const char *eol=find_char(b,s,'\n');
   if(!eol)
   {
      if(pty_recv_buf->Eof())
      {
	 if(s>0)
	    LogRecv(4,b);
	 LogError(0,_("Peer closed connection"));
      }
      if(pty_recv_buf->Error())
	 LogError(4,"pty read: %s",pty_recv_buf->ErrorText());
      if(pty_recv_buf->Eof() || pty_recv_buf->Error()) {
	 if(last_ssh_message && time_t(now)-last_ssh_message_time<4)
	    LogError(0,"%s",last_ssh_message.get());
	 Disconnect(last_ssh_message);
      }
      return;
   }
   s=eol-b+1;
   int chomp_cr=(s>=2 && b[s-2]=='\r');
   last_ssh_message.nset(b,s-1-chomp_cr);
   last_ssh_message_time=now;
   pty_recv_buf->Skip(s);
   LogRecv(4,last_ssh_message);
   if(last_ssh_message.begins_with("ssh: "))
      last_ssh_message.set(last_ssh_message+5);

   if(!received_greeting && last_ssh_message.eq(greeting))
      received_greeting=true;
}