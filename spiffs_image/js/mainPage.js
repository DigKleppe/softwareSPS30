var mcData;
var ncData;

var chartRdy = false;
var tick = 0;

var chartHeigth = 500;
var simValue1 = 0;
var simValue2 = 0;
var table;
var presc = 1;
var simMssgCnts = 0;
var lastTimeStamp = 0;
var REQINTERVAL = (5 * 60); // sec
var firstRequest = true;
var plotTimer = 6; // every 60 seconds plot averaged value
var rows = 0;

var SECONDSPERTICK = (5 * 60);// log interval 
var LOGDAYS = 1;
var MAXPOINTS = (LOGDAYS * 24 * 60 * 60 / SECONDSPERTICK)

var mcLabelTxt = ["mc 1p0", "mc 2p5", "mc 4p0","mc 10p0"];

var NRFields = 4; // timestamp, co2, t , rh 

var mcOptions = {
	title: '',
	curveType: 'function',
	legend: { position: 'top' },
	heigth: 200,
	crosshair: { trigger: 'both' },	// Display crosshairs on focus and selection.
	explorer: {
		actions: ['dragToZoom', 'rightClickToReset'],
		//actions: ['dragToPan', 'rightClickToReset'],
		axis: 'horizontal',
		keepInBounds: true,
		maxZoomIn: 100.0
	},
	chartArea: { 'width': '90%', 'height': '80%' },

	vAxes: {
		0: { logScale: false },
		1: { logScale: false },
	},
	series: {
		0: { targetAxisIndex: 0 },
		1: { targetAxisIndex: 0 },
		2: { targetAxisIndex: 0 },
		3: { targetAxisIndex: 0 },
		
	},
};

function clear() {
	mcData.removeRows(0, mcData.getNumberOfRows());
	mcChart.draw(mcData, mcOptions);
}

//var formatter_time= new google.visualization.DateFormat({formatType: 'long'});
// channel 1 .. 5

function initChart() {
	mcChart = new google.visualization.LineChart(document.getElementById('mcChart'));
	mcData = new google.visualization.DataTable();
	mcData.addColumn('string', 'Time');
	for ( var m =0 ; m < mcLabelTxt.length; m++ ) 
		mcData.addColumn('number', mcLabelTxt[m]);

	if (SIMULATE) {
		simplot();
	}
	//SIMULATE = true;	
	startTimer();
}

function startTimer() {
	setInterval(function () { timer() }, 1000);
}

// plots one sample ( array of 10 items)
function plot(values, timeStamp) {
	var row;
	var item;
	mcData.addRow();
	row = mcData.getNumberOfRows();
	if (row > MAXPOINTS ) {
		mcData.removeRows(0, 1);
		row--;
	}
	var date = new Date(timeStamp);
	var labelText = date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
	row--;

	mcData.setValue(row , 0, labelText);
	item = 1; // item 0 is timestamp
	for ( var m =0 ; m < mcLabelTxt.length; m++ ) {  // plot 4 values in mcChart
		var value = parseFloat(values[item]);
		mcData.setValue(row, item, value);
		item++;
	}
}
    
function plotLog(str) {
	var arr;
	var timeOffset;
	var sampleTime;
	var measTimeLastSample;
	var arr2 = str.split("\n");

	var nrPoints = arr2.length - 1;  
	if (nrPoints > 0) {
		var arr = arr2[nrPoints - 1].split(",");   
		measTimeLastSample = arr[0];
			
		var sec = Date.now();//  / 1000;  // mseconds since 1-1-1970 
		timeOffset = sec - parseFloat(measTimeLastSample) * 1000;

		for (var p = 0; p < nrPoints; p++) {
			arr = arr2[p].split(",");   
			if (arr.length >= NRFields) {
				sampleTime = parseFloat(arr[0]) * 1000 + timeOffset;
				plot( arr, sampleTime);
			}
		}
		mcChart.draw(mcData, mcOptions);
	}
}

function simplot() {
}

function timer() {
	var arr;
	var str;

	if (SIMULATE) {
		//	simplot();
	}
	else {
		if (firstRequest) {
			arr = getItem("getLogMeasValues");
			mcData.removeRows(0, mcData.getNumberOfRows());
			plotLog(arr);
			firstRequest = false;
		}
		str = getItem("getRTMeasValues"); // request data from enabled sensors
		if (str) {
			arr = str.split(",");
			if (arr.length >= 3) {
				if (arr[1] > 0) {
					if (arr[1] != lastTimeStamp) {
						lastTimeStamp = arr[1];
						plotLog(str);
					}
				}
			}
		}
		else
			console.log("getRTMeasValues failed");
	}
}

function clearChart() {
	mcData.removeRows(0, mcData.getNumberOfRows());
	mcChart.draw(mcData, mcOptions);
}

function clearLog() {
	getItem("clearLog");
	clearChart();
}

function refreshChart() {
	mcData.removeRows(0, mcData.getNumberOfRows());
	arr = getItem("getLogMeasValues");
	plotArray(arr);
}
