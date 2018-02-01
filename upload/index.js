const SerialPort = require('serialport');
const {spawn} = require('child_process');
var before = null;
var diff = null;
var loop;

function loopFunction() {
	SerialPort.list().then(ports => {
		ports.forEach((val, index) => {
			ports[index] = val.comName;
		});
		if (before === null) {
			before = ports;
			return;
		}
		diff = ports.filter(port => before.indexOf(port) < 0);
		console.log("Before:");
		console.log(before);
		console.log("Now:");
		console.log(ports);
		console.log("Diff:");
		console.log(diff);
		if (diff.length > 0) {
			clearInterval(loop);
			upload(diff[0]);
			return;
		}
		before = ports;
	});
}

function upload(port) {
	console.log(port);
	setTimeout(()=>{
		var proc = spawn("/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude",
			//var proc = spawn("/bin/echo",
			["-C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf",
				"-v",
				"-v",
				"-patmega2560",
				"-cstk500v2",
				"-P"+port,
				"-b57600",
				"-Uflash:w:" + process.argv[3] + ":i", "-u"]);
		proc.stdout.on('data', (data) => {
			console.log(`stdout: ${data}`);
		});
		proc.stderr.on('data', (data) => {
			console.log(`stderr: ${data}`);
		});
	},500);
}

function touch(port) {
	console.log(port);
	var serialPort = new SerialPort(port, {autoOpen: false, baudRate: 1200});
	serialPort.open(function (err) {
		if (err) {
			console.error(err);
			process.exit(1);
		}
		serialPort.close(function (err) {
			if (err) {
				console.error(err);
				process.exit(1);
			}
			loop = setInterval(loopFunction, 250);
		});
	})
}

if (process.argv.length < 4) {
	console.error("Give serial port and file as argument");
	process.exit(1);
}
if(process.argv[2]==="autodetect"){
	var foundPort = null;
	SerialPort.list().then(ports=>{
		console.log(ports);
		ports.forEach(port=>{
			console.log(port);
			if(port.comName.startsWith("/dev/cu.usbmodem")){
				foundPort = port.comName;
			}
			if(port.comName.startsWith("/dev/tty.usbmodem")){
				foundPort = port.comName;
			}
		});
		touch(foundPort);
	});
}else touch(process.argv[2]);
