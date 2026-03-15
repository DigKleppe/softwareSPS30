var dayData;
var hourData;
var firstTime = true;
var INFOTABLENAME = "sensorInfoTable";
var REQUESTINTERVAL = 1;  // sec
var DAYLOGINTERVAL = (5 * 60); // min
var simValue1 = 0;
var simValue2 = 0;
var lastTimeStamp = 0;
var dayLogPrescaler = 1;
var firstRequest = true;

var SECONDSPERTICK = (5 * 60);// log interval 
var LOGDAYS = 1;
var MAXPOINTS = (LOGDAYS * 24 * 60 * 60 / SECONDSPERTICK)

var mcLabelTxt = ["PM1", "PM2.5", "PM4", "PM10"];
var averagedValues = [0, 0, 0, 0];
var nrAverages = 0;

var NRFields = 4;

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
	dayData.removeRows(0, dayData.getNumberOfRows());
	mcChart.draw(dayData, mcOptions);
}
function forgetWifi() {
	getItem("forgetWifi");
}


//var formatter_time= new google.visualization.DateFormat({formatType: 'long'});
// channel 1 .. 5

function initChart() {
	dayChart = new google.visualization.LineChart(document.getElementById('dayChart'));
	dayData = new google.visualization.DataTable();
	dayData.addColumn('string', 'Time');
	for (var m = 0; m < mcLabelTxt.length; m++)
		dayData.addColumn('number', mcLabelTxt[m]);

	hourChart = new google.visualization.LineChart(document.getElementById('hourChart'));
	hourData = new google.visualization.DataTable();
	hourData.addColumn('string', 'Time');
	for (var m = 0; m < mcLabelTxt.length; m++)
		hourData.addColumn('number', mcLabelTxt[m]);


	if (SIMULATE) {
		simplot();
	}
	//SIMULATE = true;	
	timer();
	startTimer();
}

function startTimer() {
	setInterval(function () { timer() }, REQUESTINTERVAL * 1000);
}

// plots one sample ( array of 10 items)
function plot(chartData, values, timeStamp) {
	var row;
	var item;
	chartData.addRow();
	row = chartData.getNumberOfRows();
	if (row > MAXPOINTS) {
		chartData.removeRows(0, 1);
		row--;
	}
	var date = new Date(timeStamp);
	var labelText = date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
	row--;

	chartData.setValue(row, 0, labelText);
	item = 1; // item 0 is timestamp
	for (var m = 0; m < mcLabelTxt.length; m++) {  // plot 4 values in mcChart
		var value = parseFloat(values[item]);
		chartData.setValue(row, item, value);
		item++;
	}
}

function plotLog(chart, chartData, str) {
	var arr;
	var timeOffset;
	var sampleTime;
	var measTimeLastSample;
	if (str) {
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
					plot(chartData, arr, sampleTime);
				}
			}
			chart.draw(chartData, mcOptions);
		}
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
			arr = getItem("getDayLogMeasValues");
			dayData.removeRows(0, dayData.getNumberOfRows());
			plotLog(dayChart, dayData, arr);
			arr = getItem("getHourLogMeasValues");
			hourData.removeRows(0, hourData.getNumberOfRows());
			plotLog(hourChart, hourData, arr);
			firstRequest = false;
		}
		str = getItem("getRTMeasValues");
		if (str) {
			arr = str.split(",");
			if (arr.length >= 3) {
				if (arr[1] > 0) {
					if (arr[1] != lastTimeStamp) {
						lastTimeStamp = arr[1];
						plotLog(hourChart, hourData, str);
						// average values for dayLog
						var values = str.split(",");
						for (var m = 0; m < mcLabelTxt.length; m++) {
							averagedValues[m] += parseFloat(values[m + 1]); // values[0] is timestamp
						}
						nrAverages++;

						dayLogPrescaler--;
						if (dayLogPrescaler == 0) {
							dayLogPrescaler = DAYLOGINTERVAL / REQUESTINTERVAL;
							var str;
							str = arr[0].toString() + ","; // timestamp
							for (var m = 0; m < mcLabelTxt.length; m++) {
								str += (averagedValues[m] / nrAverages).toString();  
								if (m < mcLabelTxt.length -1)
									str += ",";
								else
									str += "\n";
							}
							nrAverages = 0;
							plotLog(dayChart, dayData, str);
						}
					}
				}
			}
		}
		else
			console.log("getRTMeasValues failed");

		getInfo("getSensorInfo", INFOTABLENAME, firstTime);
		firstTime = false;
	}
}

function clearChart() {
	dayData.removeRows(0, dayData.getNumberOfRows());
	mcChart.draw(dayData, mcOptions);
}

function clearLog() {
	getItem("clearLog");
	clearChart();
}

function refreshChart() {
	dayData.removeRows(0, dayData.getNumberOfRows());
	arr = getItem("getLogMeasValues");
	plotArray(arr);
}
