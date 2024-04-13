void SSH_Access::MoveConnectionHere(SSH_Access *o)
{
   send_buf=o->send_buf.borrow();
   recv_buf=o->recv_buf.borrow();
   pty_send_buf=o->pty_send_buf.borrow();
   pty_recv_buf=o->pty_recv_buf.borrow();
   ssh=o->ssh.borrow();
   received_greeting=o->received_greeting;
   password_sent=o->password_sent;
   last_ssh_message.move_here(o->last_ssh_message);
   last_ssh_message_time=o->last_ssh_message_time; o->last_ssh_message_time=0;
}