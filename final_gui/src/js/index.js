

window.addEventListener('load', onLoad);

var cmdPacket = new Uint8Array(6); // Command data packet, 'c' + # + (4 data bytes)

// Websocket Globals
var gateway1 = `ws://${window.location.hostname}:80/ws`;
var gateway2 = `ws://${window.location.hostname}:81/ws`;
//var websocket1;
//var websocket2;

var timeDivOS = 0.05;
var timeDivOS2 = 0.05;


/*
 * When page loads, intialize the x_value array for the oscilloscopes. Then initialize the websockets.
 *
 */
function onLoad(event) {
	cmdPacket[0] = 99;
	
	for (let i = 0; i < 1023; i++){
		x_value[i] = i;
	}	
	
	initWebSocket1();	// Initialize websocket 1 (UTF-8) and websocket 2 (binary).
	initWebSocket2();
	
	// Debugging Set periodic function in logic.js
	//setInterval(periodicUpdateLA, 10);
	//setInterval(perioLA, 20);
}

/* 
 * Init websocket 1. Used to transmit commands back to the ESP32.
 * At this time, messages on websocket 1 are not expected.
 *
 */
function initWebSocket1() {
	console.log('Trying to open a WebSocket connection...');
	websocket1 = new WebSocket(gateway1);
	websocket1.binaryType = 'arraybuffer';
	websocket1.onopen    = onOpen1;
	websocket1.onclose   = onClose1;
	websocket1.onmessage = onMessage1;
}
function onOpen1(event) {
	console.log('Connection opened');
}
function onClose1(event) {
	console.log('Connection closed');
	setTimeout(initWebSocket1, 2000);
}
function onMessage1(event) {
}

/* 
 * Init websocket 2. Used to recieve data. Since we are receiving a lot of
 * data, binary is utilized. The data is organized as an arraybuffer. Element
 * 1 is the sentinal character and tells the websocket which bit of data is
 * being received. For instance, buffer[0] = 1, indicates oscilloscope channel 1.
 *
 */
function initWebSocket2() {
	console.log('Trying to open a WebSocket 2 connection...');
	websocket2 = new WebSocket(gateway2);
	websocket2.binaryType = 'arraybuffer';
	websocket2.onopen    = onOpen2;
	websocket2.onclose   = onClose2;
	websocket2.onmessage = onMessage2;
	websocket2.onerror = onError2;
}
function onOpen2(event) {
	console.log('Connection 2 opened');
}
function onClose2(event) {
	console.log('Connection 2 closed');
	setTimeout(initWebSocket2, 2000);
}

/*
 * Data is received and placed in buffer. Buffer is then sent
 * to be handled by handleDataPacket.
 *
 */
function onMessage2(event) {
	var buffer = new Uint8Array(event.data);
	
	handleDataPacket(buffer);
	sendCommand(1, 0);
	//console.log(buffer);	// Debugging
}

function onError2(event){
	console.log("Websocket Error: ", event);
}

/*
 * Send command number with command value. Packet is constructed
 * and then sent out of websocket 1.
 *
 */
function sendCommand(cmdNum, cmdVal){
	cmdPacket[1] = cmdNum;
	cmdPacket[5] = cmdVal & 0xFF;
	cmdPacket[4] = (cmdVal >> 8) & 0xFF;
	cmdPacket[3] = (cmdVal >> 16) & 0xFF;
	cmdPacket[2] = (cmdVal >> 24) & 0xFF;
	websocket1.send(cmdPacket);
}

/*
 * The buffer sentinal character is checked to see which
 * kind of data packet was received and then handles the
 * data parsing accordingly.
 *
 */
function handleDataPacket(buf){
	
	switch(buf[0]){
		case 1: // Oscope channel 1 data packet
			var i = 0;
			for(i = 1; i < 1024; i++){
				x_value[i - 1] = ((i)*((timeDivOS*10)/1023)).toPrecision(3);
				y_value[i - 1] = buf[i] * (3.30 / 256.00);
			}
			pushDataOS();
			break;
		case 2:	// Oscope channel 2 data packet
			var i = 0;
			for(i = 1; i < 1024; i++){
				x_value[i - 1] = ((i)*((timeDivOS2*10)/1023)).toPrecision(3);
				y_value2[i - 1] = buf[i] * (3.30 / 256.00);
			}
			pushDataOS2();
			break;
		case 3:	// LA data packet
			logicChar = buf[1];
			break;
		case 4:	// DMM data packet
			var voltage, current, power = 0;
			voltage = buf[1];
			voltage = (voltage << 8) | buf[2];
			current = buf[3];
			current = (current << 8) | buf[4];
			power 	= buf[5];
			power	= (power << 8) | buf[6];
			updateDMM(voltage, current, power);
			break;
		default:
			break;
	}
}


// -- Digital Multimeter ------------------------------------------------------------------------------------------------

var displayDMMData = false;

/*
 * Receives Voltage, Current, and Power and calculates Resistance. Scaling is
 * determined by the calibration value used in the TI Innovator HUB firmware (0x13B2).
 * The value can be found in multimeter.h.
 *
 */
function updateDMM(voltage, current, power) {
    
	if(!displayDMMData){
		return;
	}
	var voltage2 = voltage / 800.0;
	var current2 = current * 0.01;
	var power2 = power * 0.25;
	document.getElementById('dmm_voltage').value	= voltage2.toFixed(2) + " V";
	document.getElementById('dmm_current').value	= current2.toFixed(2) + " mA";
	document.getElementById('dmm_power').value		= power2.toFixed(2) + " mW";
	
	var resistance = (voltage2 / current2) * 1000;
	document.getElementById('dmm_resistance').value		= resistance.toFixed(0) + " Ω";
}

// Start DMM
function startDMM() {
	// Send off websocket command c2.
	sendCommand(2, 1)
    displayDMMData = true;
}

// Stop DMM
function stopDMM() {
	// Send off websocket command c3.
	sendCommand(2, 0)
    displayDMMData = false;
}

// -- Oscilloscope ------------------------------------------------------------------------------------------------------

// Global Variables
var chartOS = generateOSChart();
var chartOS2 = generateOSChart2();

var voltDivCh1 = 1.00;
var voltDivCh2 = 1.00;
var voltOffCh1 = 0.00;
var voltOffCh2 = 0.00;

var displayOSData1 = false;
var displayOSData2 = false;

var y_value = [];
var x_value = [];
var y_value2 = [];

// Sets up the char.js chart that is called oscilloscopeChart
function generateOSChart(xValues, yValues){	

	var ctx = document.getElementById("oscilloscopeChart").getContext("2d");

	var chart = new Chart(ctx, {
	  type: "line",
	  data: {
		labels: xValues,
		datasets: [
		  {
			label: "Voltage Ch 1",
			pointStyle: "line",
			data: yValues,
			steppedLine: false,
			fill: false,
			borderColor: "rgb(220, 53, 69)",
		  }/*,
		  {
			label: "Voltage Ch 2",
			pointStyle: "line",
			data: [],
			steppedLine: false,
			fill: false,
			borderColor: "rgb(220, 53, 69)",
		  }*/
		],
	  },
	  options: {
		plugins: {
		  zoom: {
			pan: {
			  enabled: true,
			  mode: "x",
			},
			zoom: {
			  enabled: true,
			  mode: "x",
			},
		  },
		},
		title: {
		  display: true,
		  text: "Oscilloscope",
		  fontColor: "rgb(255, 255, 255)",
		},
		animation: {
			duration: 0
		},
		legend: {
		  labels: {
			defaultFontFamily:
			  "'Roboto', 'Franklin Gothic Medium', 'Tahoma', 'sans-serif",
			fontColor: "rgb(255, 255, 255)",
		  },
		},
		color: "rgb(255, 255, 255)",
		scales: {
		  yAxes: [
			{
			  gridLines: {
				drawOnChartArea: true,
				color: "rgb(255,255,255)",
			  },
			  ticks: {
				fontColor: "rgb(255, 255, 255)",
				beginAtZero: true,
				steps: 5,
				min: voltOffCh1,
				max: ((voltDivCh1*5) + voltOffCh1),
				callback: function (value, index, values) {
				  return value + " V";
				},
			  },
			  scaleLabel: {
				display: true,
				labelString: "Voltage",
			  },
			  type: "linear",
			},
		  ],
		  xAxes: [
			{
			  gridLines: {
				drawOnChartArea: false,
			  },
			  ticks: {
				fontColor: "rgb(255, 255, 255)",
				beginAtZero: true,
				//steps: 10,
				//max: (timeDivOS * 10),
				callback: function (value, index, values) {
					  return value + ' s';
				},
			  },
			  scaleLabel: {
				display: true,
				labelString: "Time",
			  },
			  //type: "linear", // This breaks chart	 
			},
		  ],
		},
	  },
	});
	return chart;
}

function generateOSChart2(xValues, yValues){	

	var ctx = document.getElementById("oscilloscopeChart2").getContext("2d");

	var chart = new Chart(ctx, {
	  type: "line",
	  data: {
		labels: xValues,
		datasets: [
		  /*{
			label: "Voltage Ch 1",
			pointStyle: "line",
			data: yValues,
			steppedLine: false,
			fill: false,
			borderColor: "rgb(14, 173, 105)",
		  },*/
		  {
			label: "Voltage Ch 2",
			pointStyle: "line",
			data: yValues,
			steppedLine: false,
			fill: false,
			borderColor: "rgb(220, 53, 69)",
		  }
		],
	  },
	  options: {
		plugins: {
		  zoom: {
			pan: {
			  enabled: true,
			  mode: "x",
			},
			zoom: {
			  enabled: true,
			  mode: "x",
			},
		  },
		},
		title: {
		  display: true,
		  text: "Oscilloscope",
		  fontColor: "rgb(255, 255, 255)",
		},
		animation: {
			duration: 0
		},
		legend: {
		  labels: {
			defaultFontFamily:
			  "'Roboto', 'Franklin Gothic Medium', 'Tahoma', 'sans-serif",
			fontColor: "rgb(255, 255, 255)",
		  },
		},
		color: "rgb(255, 255, 255)",
		scales: {
		  yAxes: [
			{
			  gridLines: {
				drawOnChartArea: true,
				color: "rgb(255,255,255)",
			  },
			  ticks: {
				fontColor: "rgb(255, 255, 255)",
				beginAtZero: true,
				steps: 5,
				min: voltOffCh2,
				max: ((voltDivCh2*5)+voltOffCh2),
				callback: function (value, index, values) {
					return value + " V";
				},
			  },
			  scaleLabel: {
				display: true,
				labelString: "Voltage",
			  },
			  type: "linear",
			},
		  ],
		  xAxes: [
			{
			  gridLines: {
				drawOnChartArea: false,
			  },
			  ticks: {
				fontColor: "rgb(255, 255, 255)",
				beginAtZero: true,
				//steps: 10,
				//max: (timeDivOS * 10),
				callback: function (value, index, values) {
				  return value + ' s';
				},
			  },
			  scaleLabel: {
				display: true,
				labelString: "Time",
			  },
			  //type: "linear", // This breaks chart	 
			},
		  ],
		},
	  },
	});
	return chart;
}

/*
 * We destroy the chart and then recreate the chart to update the plot.
 *
 */
function pushDataOS() {
	chartOS.destroy();
	chartOS = generateOSChart(x_value, y_value);
}

function pushDataOS2() {
	chartOS2.destroy();
	chartOS2 = generateOSChart2(x_value, y_value2);
}

/*
 * Handles user input into volt per division input textbox.
 *
 */
function updateVoltDivCh1(){
	voltDivCh1 = parseFloat(document.getElementById('voltDivCh1').value);
	if(isNaN(voltDivCh1)){
		return;
	}
	if(document.getElementById('voltDivUnitCh1').value == "mV"){
		document.getElementById('voltDivCh1').value = voltDivCh1;
		voltDivCh1 = voltDivCh1 / 1000.0;
	}
	else{
		document.getElementById('voltDivCh1').value = voltDivCh1;
	}
}

/*
 * Handles user input into voltage offset input textbox.
 *
 */
function updateVoltOffCh1(){
	voltOffCh1 = parseFloat(document.getElementById('voltOffCh1').value);
	if(isNaN(voltOffCh1)){
		return;
	}
	if(document.getElementById('voltOffUnitCh1').value == "mV"){
		document.getElementById('voltOffCh1').value = voltOffCh1;
		voltOffCh1 = voltOffCh1 / 1000.0;
	}
	else{
		document.getElementById('voltOffCh1').value = voltOffCh1;
	}	
}

/*
 * Updates voltage per division when the unit is changed between mV and V.
 *
 */
function updateVoltUnitCh1(divOrOff){
	if(divOrOff){
		var tempVoltDiv = parseFloat(document.getElementById('voltDivCh1').value);
		if(isNaN(tempVoltDiv)){
			return;
		}
		if(document.getElementById('voltDivUnitCh1').value == "mV"){
			document.getElementById('voltDivCh1').value = tempVoltDiv * 1000.00;
		}
		else{
			document.getElementById('voltDivCh1').value = tempVoltDiv / 1000.00;
		}
	} else{
		var tempVoltOff = parseFloat(document.getElementById('voltOffCh1').value);
		if(isNaN(tempVoltOff)){
			return;
		}
		if(document.getElementById('voltOffUnitCh1').value == "mV"){
			document.getElementById('voltOffCh1').value = tempVoltOff * 1000.00;
		}
		else{
			document.getElementById('voltOffCh1').value = tempVoltOff / 1000.00;
		}
	}
}

/*
 * Handles user input into volt per division input textbox.
 *
 */
function updateVoltDivCh2(){
	voltDivCh2 = parseFloat(document.getElementById('voltDivCh2').value);
	if(isNaN(voltDivCh2)){
		return;
	}
	if(document.getElementById('voltUnitCh2').value == "mV"){
		document.getElementById('voltDivCh2').value = voltDivCh2;
		voltDivCh2 = voltDivCh2 / 1000.0;
	}
	else{
		document.getElementById('voltDivCh2').value = voltDivCh2;
	}
}

/*
 * Handles user input into voltage offset input textbox.
 *
 */
function updateVoltOffCh2(){
	voltOffCh2 = parseFloat(document.getElementById('voltOffCh2').value);
	if(isNaN(voltOffCh2)){
		return;
	}
	if(document.getElementById('voltOffUnitCh2').value == "mV"){
		document.getElementById('voltOffCh2').value = voltOffCh2;
		voltOffCh2 = voltOffCh2 / 1000.0;
	}
	else{
		document.getElementById('voltOffCh2').value = voltOffCh2;
	}	
}

/*
 * Updates voltage per division when the unit is changed between mV and V.
 *
 */
function updateVoltUnitCh2(divOrOff){
	if(divOrOff){
		var tempVoltDiv = parseFloat(document.getElementById('voltDivCh2').value);
		if(isNaN(tempVoltDiv)){
			return;
		}
		if(document.getElementById('voltDivUnitCh2').value == "mV"){
			document.getElementById('voltDivCh2').value = tempVoltDiv * 1000.00;
		}
		else{
			document.getElementById('voltDivCh2').value = tempVoltDiv / 1000.00;
		}
	} else{
		var tempVoltOff = parseFloat(document.getElementById('voltOffCh2').value);
		if(isNaN(tempVoltOff)){
			return;
		}
		if(document.getElementById('voltOffUnitCh2').value == "mV"){
			document.getElementById('voltOffCh2').value = tempVoltOff * 1000.00;
		}
		else{
			document.getElementById('voltOffCh2').value = tempVoltOff / 1000.00;
		}		
	}
}

/*
 * Handles time per division for oscilloscope channel 1.
 *
 */
function updateTimeDivOS(){
	timeDivOS = parseFloat(document.getElementById('timeDivOS').value);
	if(isNaN(timeDivOS)){
		return;
	}
	if(document.getElementById('timeUnitOS').value == "mSec"){
		document.getElementById('timeDivOS').value = timeDivOS;
		timeDivOS = timeDivOS / 1000.0;
	} else if(document.getElementById('timeUnitOS').value == "uSec"){
		document.getElementById('timeDivOS').value = timeDivOS;
		timeDivOS = timeDivOS / 1000000.0;		
	}
	else{
		document.getElementById('timeDivOS').value = timeDivOS;
	}
	sendCommand(6, Math.round(((timeDivOS*10)/1023)*6000000));
	//console.log(Math.round(((timeDivOS*10)/1023)*6000000));
}

function updateTimeUnitOS(){
	tempDivOS = parseFloat(document.getElementById('timeDivOS').value);
	if(isNaN(tempDivOS)){
		return;
	}
	if(document.getElementById('timeUnitOS').value == "mSec"){
		document.getElementById('timeDivOS').value = tempDivOS * 1000.00;
	} else if(document.getElementById('timeUnitOS').value == "uSec"){
		document.getElementById('timeDivOS').value = tempDivOS * 1000000.00;
	}
	else{
		document.getElementById('timeDivOS').value = tempDivOS / 1000.00;
	}
}

/*
 * Handles time per division for oscilloscope channel 2.
 *
 */
function updateTimeDivOS2(){
	timeDivOS2 = parseFloat(document.getElementById('timeDivOS2').value);
	if(isNaN(timeDivOS2)){
		return;
	}
	if(document.getElementById('timeUnitOS2').value == "mSec"){
		document.getElementById('timeDivOS2').value = timeDivOS;
		timeDivOS2 = timeDivOS2 / 1000.0;
	} else if(document.getElementById('timeUnitOS2').value == "uSec"){
		document.getElementById('timeDivOS2').value = timeDivOS2;
		timeDivOS2 = timeDivOS2 / 1000000.0;		
	}
	else{
		document.getElementById('timeDivOS2').value = timeDivOS2;
	}
	sendCommand(7, Math.round(((timeDivOS2*10)/1023)*6000000));
	//console.log(Math.round(((timeDivOS*10)/1023)*6000000));
}

function updateTimeUnitOS2(){
	tempDivOS2 = parseFloat(document.getElementById('timeDivOS2').value);
	if(isNaN(tempDivOS2)){
		return;
	}
	if(document.getElementById('timeUnitOS2').value == "mSec"){
		document.getElementById('timeDivOS2').value = tempDivOS2 * 1000.00;
	} else if(document.getElementById('timeUnitOS2').value == "uSec"){
		document.getElementById('timeDivOS2').value = tempDivOS2 * 1000000.00;
	}
	else{
		document.getElementById('timeDivOS2').value = tempDivOS2 / 1000.00;
	}
}

// Toggle the data on.
function startOS1() {
	sendCommand(3, 1)
	sendCommand(1, 0);
	displayOSData1 = true;
}

// Toggle the data off.
function stopOS1() {
	sendCommand(3, 0)
	displayOSData1 = false;
}

// Toggle the data on.
function startOS2() {
	sendCommand(4, 1)
	displayOSData2 = true;
}

// Toggle the data off.
function stopOS2() {
	sendCommand(4, 0)
	displayOSData2 = false;
}

// -- Function Generator ------------------------------------------------------------------------------------------------

var chartFG = generateFGChart();

var channelFG = 0;

var freqFG = [5,5,5,5];
var ampFG =  [1,1,1,1];
var dacfreqFG = 1024;
var typeFG = [0,0,0,0];
var biasFG = [1,1,1,1];

/*
 * Generates function generator chart that previews the expected signal.
 *
 */
function generateFGChart() {
	
	var chart = new Chart(document.getElementById("idealChart").getContext('2d'), {
		type: 'line',
		data: {
			datasets: [{
				pointStyle: 'line',
				label: "Ideal voltage",
				data: [],
				steppedLine: false,
				fill: false,
				borderColor: 'rgb(220, 53, 69)',
			}]
		},
		options: {
			plugins: {
				zoom: {
					pan: {
						enabled: true,
						mode: 'x'
					},
					zoom: {
						enabled: true,
						mode: 'x'
					}
				}
			},
			title: {
				display: true,
				text: 'Waveform',
				fontColor: 'rgb(255, 255, 255)',
			},
			legend: {
				display: false,
				labels: {
					defaultFontFamily: "'Roboto', 'Franklin Gothic Medium', 'Tahoma', 'sans-serif",
					fontColor: 'rgb(255, 255, 255)',
				}
			},
			color: 'rgb(255, 255, 255)',
			scales: {
				yAxes: [{
					gridLines: {
						drawOnChartArea: true,
						color: 'rgb(255,255,255)',
					},
					ticks: {
						fontColor: 'rgb(255, 255, 255)',
						beginAtZero: true,
						callback: function (value, index, values) {
							return value + ' V';
						}
					},
					scaleLabel: {
						display: true,
						labelString: 'Voltage'
					},
					type: 'linear'
				}],
				xAxes: [{
					ticks: {
						fontColor: 'rgb(255, 255, 255)',
						beginAtZero: true,
						callback: function (value, index, values) {
							return value + ' s';
						}
					},
					scaleLabel: {
						display: true,
						labelString: 'Time'
					},
					type: 'linear'
				}]
			}
		}
	});
	return chart;
}

function trimNum(num) {
    return Math.round(num * 10 ** 7) / 10 ** 7;
}

/*
 * Constructs the data that will be used to generate the function generator plot
 * that is used to preview the expected signal.
 *
 */
function setIdealChart(freq, amplitude, dacFreq, type, bias) {
	var numSamples = 1000;
	var tVal = [];
	var vVal = [];
	var period = 1/ freq;
	var dt = (period / numSamples);
    // Generate 1 cycle of waveform
    switch (type) {
        // Square
        case 0:
			var t;
			vVal.push(bias - (amplitude / 2));
			tVal.push(0);
            for (i = 1; i < (numSamples-1); i++) {
				t = i*dt
				if(i < (numSamples / 2)){
					vVal.push(bias + (amplitude / 2));
				} else {
					vVal.push(bias - (amplitude / 2));
				}
				tVal.push(t);
            }
			vVal.push(bias + (amplitude / 2));
			tVal.push((numSamples - 1)*dt);			
            break;
        // Sine
        case 1:
			var t;
            for (i = 0; i < numSamples; i++) {
				t = i*dt
				vVal.push((amplitude / 2) * Math.sin(2*Math.PI*freq*t) + bias);
				tVal.push(t);
            }
            break;
        // Sawtooth
        case 2:
            var t;
			var dv = amplitude / 1000;
            for (i = 0; i < (numSamples - 1); i++) {
				t = i*dt
				vVal.push(i*dv + bias - (amplitude / 2));
				tVal.push(t);
            }
			vVal.push(bias - (amplitude / 2));
			tVal.push((numSamples - 1)*dt);			
            break;
		case 3:
			var t;
            for (i = 0; i < numSamples; i++) {
				t = i*dt
				vVal.push(bias);
				tVal.push(t);
            }
			break;
		default:
			break;
    }


    chartFG.data.datasets[0].data = [];

	for(i = 0; i < numSamples; i++){
		chartFG.data.datasets[0].data.push({ x: tVal[i], y: vVal[i] });
	}
    chartFG.update();
}

$(document).ready(function () {
    setIdealChart(freqFG[channelFG], ampFG[channelFG], dacfreqFG, typeFG[channelFG], biasFG[channelFG]);
});

/*
 * These functions are used to parse the input from the user and to send its value in a command
 * to the TI Innovator HUB.
 *
 */
function updateFreqFG(){
	freqFG[channelFG] = document.getElementById('freqFG').value;
	freqFG[channelFG] = freqFG[channelFG] <= 0 ? 0.01 : freqFG[channelFG];
	document.getElementById('freq-input').value = freqFG[channelFG];
	setIdealChart(freqFG[channelFG], ampFG[channelFG], dacfreqFG, typeFG[channelFG], biasFG[channelFG]);
	
	console.log(Math.round((1/freqFG[channelFG])*(1/128)*(750000)));
	sendCommand((17+channelFG), Math.round((1/freqFG[channelFG])*(1/128)*(750000)));
}

/*
 * These functions are used to parse the input from the user and to send its value in a command
 * to the TI Innovator HUB.
 *
 */
function updateAmpFG(){
	ampFG[channelFG] = parseFloat(document.getElementById('ampFG').value);
	document.getElementById('amplitude-input-fg').value = ampFG[channelFG];
	setIdealChart(freqFG[channelFG], ampFG[channelFG], dacfreqFG, typeFG[channelFG], biasFG[channelFG]);
	
	var temp = parseInt(ampFG[channelFG]*(65535/3.3));
	sendCommand((13+channelFG), temp);
}

/*
 * These functions are used to parse the input from the user and to send its value in a command
 * to the TI Innovator HUB.
 *
 */
function updateTypeFG(){
	typeFG[channelFG] = document.getElementById('typeFG').selectedIndex;
	setIdealChart(freqFG[channelFG], ampFG[channelFG], dacfreqFG, typeFG[channelFG], biasFG[channelFG]);
	
	sendCommand((9+channelFG), typeFG[channelFG]);
}

/*
 * These functions are used to parse the input from the user and to send its value in a command
 * to the TI Innovator HUB.
 *
 */
function updateBiasFG(){
	biasFG[channelFG] = parseFloat(document.getElementById('biasFG').value);
	document.getElementById('bias-input').value = biasFG[channelFG];
	setIdealChart(freqFG[channelFG], ampFG[channelFG], dacfreqFG, typeFG[channelFG], biasFG[channelFG]);
	
	var temp = parseInt(biasFG[channelFG]*(65535/3.3));
	//console.log(temp);
	sendCommand((21+channelFG), temp);
}

/*
 * These functions are used to parse the input from the user and to send its value in a command
 * to the TI Innovator HUB.
 *
 */
function changeChFG(){
	channelFG = document.getElementById('chFG').selectedIndex;
	document.getElementById('typeFG').selectedIndex = typeFG[channelFG];
	document.getElementById('freq-input').value = freqFG[channelFG];
	document.getElementById('amplitude-input-fg').value = ampFG[channelFG];
	document.getElementById('bias-input').value = biasFG[channelFG];	
	setIdealChart(freqFG[channelFG], ampFG[channelFG], dacfreqFG, typeFG[channelFG], biasFG[channelFG]);
}

/*
 * The value sent with the start and stop commands are chosen specifically to line up with the
 * the command bytes for each function generator channel. For instance, 0x12 is the command for
 * starting function generator channel 2. So 0x32 is (0x20 | 0x12), which allows us to know that
 * we want to turn off (0x20) channel 2 (0x12).
 *
 */
function startFG(){
	switch(channelFG){
		case 0:
			sendCommand(5, 0x10);
			break;
		case 1:
			sendCommand(5, 0x12);
			break;
		case 2:
			sendCommand(5, 0x14);
			break;
		case 3:
			sendCommand(5, 0x16);
			break;
		default:
			break;
	}
}

function stopFG(){
	switch(channelFG){
		case 0:
			sendCommand(5, 0x30);
			break;
		case 1:
			sendCommand(5, 0x32);
			break;
		case 2:
			sendCommand(5, 0x34);
			break;
		case 3:
			sendCommand(5, 0x36);
			break;
		default:
			break;
	}	
}

// -- Logic Analyzer ----------------------------------------------------------------------------------------------------

var chartLA = generateLAChart();
var activeChannels = 0x00;
var logicChar = 0;
var perio_count = 0;
var intervalIDLA = 0;

function hideChannel(num) {

	mask = 0x01 << num;
	var buttonHit = document.getElementById("channelToggle" + num);
	
	if(activeChannels & mask){
		activeChannels &= ~mask;
		buttonHit.style.background = '#c00';
		buttonHit.style.color = 'white';
	} else{
		activeChannels |= mask;
		buttonHit.style.background = 'white';
		buttonHit.style.color = '#c00';
	}
}

function pauseAll() {
    var pauseID = document.getElementById("pauseLA");
	
	if(pauseID.textContent == "Paused"){
		pauseID.textContent = "Running";
		pauseID.style.background = 'white';
		pauseID.style.color = '#c00';
		intervalIDLA = setInterval(periodicUpdateLA, 10);
	} else if(pauseID.textContent == "Running"){
		pauseID.textContent = "Paused"
		pauseID.style.background = '#c00';
		pauseID.style.color = 'white';
		clearInterval(intervalIDLA);
	}
}


function generateLAChart() {
    var ctx = document.getElementById("channelChart").getContext('2d');

	var chart = new Chart(ctx, {
	  type: "line",
	  data: {
		//labels: xValues,
		datasets: [
			{
				label: 'Channel 1',
				pointStyle: 'line',
				data: [],
				steppedLine: true,
				fill: false,
				borderColor: 'rgb(220, 53, 69)',
			},
			{
				label: 'Channel 2',
				pointStyle: 'line',
				data: [],
				steppedLine: true,
				fill: false,
				borderColor: 'rgb(220, 53, 69)',
			},
			{
				label: 'Channel 3',
				pointStyle: 'line',
				data: [],
				steppedLine: true,
				fill: false,
				borderColor: 'rgb(220, 53, 69)',
			},
			{
				label: 'Channel 4',
				pointStyle: 'line',
				data: [],
				steppedLine: true,
				fill: false,
				borderColor: 'rgb(220, 53, 69)',
			},
			{
				label: 'Channel 5',
				pointStyle: 'line',
				data: [],
				steppedLine: true,
				fill: false,
				borderColor: 'rgb(220, 53, 69)',
			},
			{
				//label: 'Channel 6',
				pointStyle: 'line',
				data: [],
				steppedLine: true,
				fill: false,
				borderColor: 'rgb(220, 53, 69)',
			},
			{
				label: 'Channel 7',
				pointStyle: 'line',
				data: [],
				steppedLine: true,
				fill: false,
				borderColor: 'rgb(220, 53, 69)',
			},
			{
				label: 'Channel 8',
				pointStyle: 'line',
				data: [],
				steppedLine: true,
				fill: false,
				borderColor: 'rgb(220, 53, 69)',
			}
		],
	  },
	  options: {
		plugins: {
		  zoom: {
			pan: {
			  enabled: true,
			  mode: "x",
			},
			zoom: {
			  enabled: true,
			  mode: "x",
			},
		  },
		},
		title: {
		  display: true,
		  text: "Logic Analyzer",
		  fontColor: "rgb(255, 255, 255)",
		},
		animation: {
			duration: 0
		},
		legend: {
			display: false,
			labels: {
				defaultFontFamily:
				  "'Roboto', 'Franklin Gothic Medium', 'Tahoma', 'sans-serif",
				fontColor: "rgb(255, 255, 255)",
		  },
		},
		color: "rgb(255, 255, 255)",
		scales: {
		  yAxes: [
			{
			  gridLines: {
				drawOnChartArea: true,
				color: "rgb(255,255,255)",
			  },
			  ticks: {
				fontColor: "rgb(255, 255, 255)",
				beginAtZero: false,
				min: 0.9,
				max: 9,
				steps: 7,
				callback: function (label, index, labels) {
					if((label >= 1) && (label < 9)){
						return label;
					}
				},
			  },
			  scaleLabel: {
				display: true,
				labelString: "Channel",
			  },
			  type: "linear",
			},
		  ],
		  xAxes: [
			{
			  gridLines: {
				drawOnChartArea: false,
			  },
			  ticks: {
				beginAtZero: true,
				max: 500,
				fontColor: "rgb(255, 255, 255)",
				callback: function (value, index, values) {
					return;// value;
				},
			  },
			  scaleLabel: {
				display: true,
				labelString: "Time",
			  }
			},
		  ],
		},
	  },
	});
	return chart;
}

/*
 * When logic analyzer is enabled, this function runs every 10 ms
 * to update the LA with the most current value. 10 ms is the shortest
 * time interval with javascript.
 *
 */
function periodicUpdateLA(){
	
	var mask = 0x01;
	var i = 0;
	
	if(perio_count > 500){
		perio_count = 0;
	}
	
	for(i = 0; i < 8; i++){
		if(logicChar & mask){
			chartLA.data.datasets[i].data[perio_count] = 1.7 + i;
		}
		else {
			chartLA.data.datasets[i].data[perio_count] = 1.0 + i;
		}
		
		if(!(activeChannels & mask)){
			chartLA.data.datasets[i].data[perio_count] = NaN;
		}
		
		mask = mask << 1;
	}
	chartLA.data.labels[perio_count] = perio_count;
	
	
	chartLA.update('quiet');
	perio_count++;
}

// Debugging
/*function perioLA(){
	logicChar++;
	if(logicChar > 255){
		logicChar = 0;
	}
}*/