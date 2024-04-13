void SSH_Access::MakePtyBuffers()
{
   int fd=ssh->getfd();
   if(fd==-1)
      return;
   ssh->Kill(SIGCONT);
   send_buf=new IOBufferFDStream(new FDStream(ssh->getfd_pipe_out(),"pipe-out"),IOBuffer::PUT);
   recv_buf=new IOBufferFDStream(new FDStream(ssh->getfd_pipe_in(),"pipe-in"),IOBuffer::GET);
   pty_send_buf=new IOBufferFDStream(ssh.borrow(),IOBuffer::PUT);
   pty_recv_buf=new IOBufferFDStream(new FDStream(fd,"pseudo-tty"),IOBuffer::GET);
}