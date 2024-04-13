void SSH_Access::DisconnectLL()
{
   if(send_buf)
      LogNote(9,_("Disconnecting"));
   send_buf=0;
   recv_buf=0;
   pty_send_buf=0;
   pty_recv_buf=0;
   ssh=0;
   received_greeting=false;
   password_sent=0;
   last_ssh_message.unset();
   last_ssh_message_time=0;
}