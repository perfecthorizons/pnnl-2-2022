`use strict`

const ipc=require('node-ipc');

ipc.config.id = 'relay';
ipc.config.retry = 1500;
/*ipc.config.networkHost = 'localhost';
ipc.config.networkPort = 50007;
ipc.config.tls = {
    rejectUnauthorized:false
};*/

ipc.connectTo(
  'casper',
  '/dev/mqueue/caspercppipc',
  function(){
    ipc.of.casper.on(
      'connect',
      function(){
        ipc.log('## connected to server ##', ipc.config.delay);
        /*
        ipc.of.casper.emit(
          'message',
          'hello'
        );
        */
      }
    );
    ipc.of.casper.on(
      'disconnect',
      function(){
        ipc.log('disconnected from server');
      }
    );
    ipc.of.casper.on(
      'message',
      function(data){
        ipc.log('got a message from server : ', data);
      }
    );
  }
);