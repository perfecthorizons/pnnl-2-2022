`use strict`

const PosixMQ = require('posix-mq');
let io = require('socket.io')();

/**
 * Read from the POSIX message queue.
 */
let mq = new PosixMQ();
let n;
mq.open({
  name: '/caspercppipc',
  create: true,
  mode: '0655',
  maxmsgs: 512,
  msgsize: 1024,
});

function runForever() {
  let readbuf = new Buffer(mq.msgsize);
  //n = 0;
  let length;
  while ((length = mq.shift(readbuf)) !== false) {
    //n++;
    //console.log(`Received message (${length} bytes): `+ readbuf.toString('utf8', 0, length));
    io.emit('casper', readbuf.toString('utf8', 0, length));
  }
  //console.log(`cleared ${n} messages`);
  setTimeout(runForever, 100);
}

runForever();

/**
 * Set up the Socket.io Server.
 */
/*io.on('connection', function(socket){
  console.log('connection', socket);
});*/
io.listen(3000);
