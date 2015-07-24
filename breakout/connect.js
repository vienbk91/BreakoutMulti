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


/**
 * Thuc hien khoi tao ket noi toi mongoDB
 * Su dung Mongoose
 */
var Mongoose = require('mongoose');
var url = 'mongodb://localhost:27017/breakout';

var Schema = Mongoose.Schema;

var RoomSchema = new Schema({
	player_id : Number ,
	status : Boolean
});

/**
 * Tao 1 schema co ten la Room
 * Document room se duoc khoi tao trong bang collection "test"
 */
var Room = Mongoose.model('room' , RoomSchema);

var roomPlayer = new Array();

io.sockets.on('connection' , function(socket){
	console.log('Client connection server Succesfull');
	
	socket.on('disconnection' , function(){
		console.log('Client disconnect');
	});
	
	
	
	// Khoi tao room db
	socket.on('create_db' , function(data){
		console.log('Client Connect thanh cong toi mongoDB');		
		 
		// Thuc hien tra du lieu db ve client  tai day
		getRoomData(roomPlayer , function(){
			console.log('Gui du lieu ban dau ve cho client');
			socket.emit('create_db_end' , {room : roomPlayer});
		});
	});
	

	// Nhan yeu cau tu client, check du lieu mongodb thong qua server
	socket.on('player_connect' , function(data){
		console.log('Gui du lieu len server thanh cong');
		console.log('Data gui len server : ' , data);
		
		var dt = JSON.parse(data);
		
		for(var i = 0 ; i < 2 ; i++){
			if(dt[i]['status'] == 1){
				dt[i]['status'] = true;
				
			}else{
				dt[i]['status'] = false;
			}
			
			// Thay doi gia tri cua bien toan cuc
			roomPlayer[i]['status'] = dt[i]['status']; 
			
			updateDataRoom(dt[i] , dt[i]['status'] , function(){
				console.log('Update succesfull data room');
				console.log('Gui du lieu da update lai cho client');
				socket.emit('player_connect_end' , {room : roomPlayer});
				socket.broadcast.emit('player_connect_end' , {room : roomPlayer});

				
			});
		}
	});

	// Thuc hien check trang thai
	socket.on('realtime_check' , function(data){
		socket.emit('realtime_check_end' , {room : roomPlayer});
	});


	// Thuc hien update vi tri cua player cho cac player khac
	socket.on('send_position_player' , function(data){
		var dt = JSON.parse(data);
		socket.broadcast.emit('send_position_player_end' , dt);

	});



	socket.on('send_ball_info' , function(data){
		// Nhan du lieu ball_if dau vao
		console.log('Ball First Info : ' , data);

		var dt = JSON.parse(data);
		socket.emit('send_ball_info_end' , dt);

	});



	socket.on('send_score' , function(data){
		// Nhan score tu client
		//console.log('Score : ' , data);
		//
		var dt = JSON.parse(data);
		
		socket.broadcast.emit('send_score_end' , dt);
	});

});


createConnection(function(){
	createRoom(function(){
		console.log('Tao thanh cong db');
	});
});	


function updateDataRoom(data , status , onUpdateDataRoom){
	
	console.log('Data dung de update' , data);
	
	Room.findOne({player_id : data['player_id']} , function(error , room){
		
		room.status = status;	
		room.save(function(error , room ){
			
			console.log('Room : ' , room);
			
			if(error){
				console.log('Update faild...');
			}else{
				console.log('Update succesfull');
				onUpdateDataRoom();
			}
		});	
	});	
	
	
}



function getRoomData(data , handle){
	console.log('Data : ' , data);
	handle();
}


function createRoom(onCreate){
	
	var user1 = new Room({player_id : 1 , status : false });
	var user2 = new Room({player_id : 2 , status : false });
	
	Room.create([user1 , user2] , function(error){
		if(error){
			console.log('Create data not succesfull');
			console.log('Error :' , error);
		}else{
			console.log('Create data succesfull');
			roomPlayer.push(user1);
			roomPlayer.push(user2);
			onCreate();
		}
	});
}

function createConnection(onConnect){
	Mongoose.connect(url , function(error){
		if(error){
			console.log('Create Connection not succesfull');
			console.log('Error : ' , error);
		}else{
			console.log('Connect to MongoDB succesfull');
			onConnect();
		}
	});
}

