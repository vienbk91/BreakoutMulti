var http = require('http');
var socketIO = require('socket.io');

var port = 8080;
var ip = '127.0.0.1';

// Tao 1 server theo phuong thuc http , lang nghe cong 8080
var server = http.createServer().listen(port , ip , function(){
	console.log('Server connected at ' + ip + ':' + port);
});

console.log('HelloWorld');

/**
 * Neu khong su dung bien server thi co the thay no thanh port
 * socketIO.listen(8080) hoac
 * socketIO.listen(8080 , '127.0.0.1'); hoac
 * server = require('http').Server(); 
 * 
 */
var io = socketIO.listen(server);

var playerNum = 0;

io.sockets.on('connection' , function(socket){
	console.log('Client connection server Succesfull');
	
	playerNum++;
			
	socket.on('disconnect' , function(){
		playerNum--;
		console.log('Client disconnect succesfull');
	});
	
	//socket.emit('hello' , {value : 'Welcom'});
	socket.on('hello' , function(data){
		var dt = JSON.parse(data);
		console.log('Data : ' , data);
		console.log('Data paser : ' , dt);
		
		console.log('Value : ' , dt["value"]);
		
		if(dt["value"] == 1){
			console.log('Player 1 connected succesfull');
		}
		
		if(dt["value"] == 2){
			console.log('Player 2 connected succesfull');
		}
		
		socket.emit('hello' , {value : dt["value"]});
		
	});
	
	if(playerNum == 2){
		socket.emit('connect_end' , {value : '2'});
	}
	
	
	
});