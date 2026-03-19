
const SECONDSPERTICK = (5 * 60);// log interval 
const LOGDAYS = 1;
const MAXDAYPOINTS = (LOGDAYS * 24 * 60 * 60 / SECONDSPERTICK);
const MAXHOURPOINTS = 3600;

const INFOTABLENAME = "sensorInfoTable";
const REQUESTINTERVAL = 1;  // sec
const DAYLOGINTERVAL = (5 * 60); // min

const NRITEMS = 4;
const lineWidth = 3;


var hourChart;
var dayChart;
var lastTimeStamp = 0;
var dayLogPrescaler = 1;
var firstRequest = true;
var firstTime = true; // for table
var averagedValues = [0, 0, 0, 0];
var nrAverages = 0;

const Utils = ChartUtils.init();

function initChart() {
    window.addEventListener('DOMContentLoaded', function () {
        var ctx = document.getElementById("hourChart");
        hourChart = new Chart(ctx, configHour);
        var ctx = document.getElementById("dayChart");
        dayChart = new Chart(ctx, configDay);
        if (SIMULATE)
            //  plotTest();
            simplot();
        timer();
        startTimer();
    });
};


const data = {
    datasets: [
        {
            label: 'PM1',
            data: [],
            borderColor: Utils.CHART_COLORS.red,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.red, 0.5),
            borderWidth: lineWidth,
            borderRadius: Number.MAX_VALUE,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        },
        {
            label: 'PM2.5',
            data: [],
            borderColor: Utils.CHART_COLORS.yellow,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.orange, 0.5),
            borderWidth: lineWidth,
            borderRadius: 5,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        },
        {
            label: 'PM4',
            data: [],
            borderColor: Utils.CHART_COLORS.blue,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.blue, 0.5),
            borderWidth: lineWidth,
            borderRadius: 5,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        },
        {
            label: 'PM10',
            data: [], 
            borderColor: Utils.CHART_COLORS.green,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.green, 0.5),
            borderWidth: lineWidth,
            borderRadius: 5,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        }
    ]
}

const data2 = {
    datasets: [
        {
            label: 'PM1',
            data: [], 
            borderColor: Utils.CHART_COLORS.red,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.red, 0.5),
            borderWidth: lineWidth,
            borderRadius: Number.MAX_VALUE,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        },
        {
            label: 'PM2.5',
            data: [],
            borderColor: Utils.CHART_COLORS.yellow,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.orange, 0.5),
            borderWidth: lineWidth,
            borderRadius: 5,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        },
        {
            label: 'PM4',
            data: [],
            borderColor: Utils.CHART_COLORS.blue,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.blue, 0.5),
            borderWidth: lineWidth,
            borderRadius: 5,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        },
        {
            label: 'PM10',
            data: [],
            borderColor: Utils.CHART_COLORS.green,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.green, 0.5),
            borderWidth: lineWidth,
            borderRadius: 5,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        }
    ]
}

const configHour = {
    type: 'line',
    data: data,
    options: {
        responsive: true,
        pointStyle: false,
        animations: false,
        plugins: {
            legend: {
                position: 'top'
            },
            title: {
                display: true,
                text: 'Uurgrafiek'
            }
        }
    }
}

const configDay = {
    type: 'line',
    data: data2,
    options: {
        responsive: true,
        pointStyle: false,
        animations: false,
        plugins: {
            legend: {
                position: 'top'
            },
            title: {
                display: true,
                text: '24 uurgrafiek'
            }
        }
    }
}

function clear() {
    var nrItems = hourChart.data.labels.length;
    hourChart.data.labels.splice(0, nrItems);

    for (let idx = 0; idx < 4; idx++) {
        hourChart.data.datasets[idx].data.splice(0, nrItems);
    }
    hourChart.update();

    nrItems = dayChart.data.labels.length;

    dayChart.data.labels.splice(0, nrItems);

    for (let idx = 0; idx < 4; idx++) {
        dayChart.data.datasets[idx].data.splice(0, nrItems);
    }
    dayChart.update();
}

function clearChart() {
    clear();
}

function clearLog() {
    getItem("clearLog");
    clearChart();
}

function startTimer() {
    if (SIMULATE)
        setInterval(function () { timer() }, 500);
    else
        setInterval(function () { timer() }, REQUESTINTERVAL * 1000);
}
function forgetWifi() {
    getItem("forgetWifi");
}

// plots one sample ( array of 4 items)
function plot(chart, values, timeStamp) {
    var maxPoints;
    if ( chart == dayChart)
        maxPoints = MAXDAYPOINTS;
    else
        maxPoints = MAXHOURPOINTS;

    const data = chart.data;
    if (data.datasets.length > 0) {
        for (let index = 0; index < data.datasets.length; ++index) {
            var value = parseFloat(values[index]);
            data.datasets[index].data.push(value);
        }

        var date = new Date(timeStamp);
        var labelText = date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
        data.labels.push(labelText);

        if (data.labels.length > maxPoints) {
            data.labels.splice(0, 1);
            for (let index = 0; index < data.datasets.length; ++index) {
                data.datasets[index].data.splice(0, 1);
            }
        }
    }
}

function plotLog(chart, str) {
    var arr;
    var timeOffset;
    var sampleTime;
    var measTimeLastSample;

    if (chart == dayChart)
        console.log("Log daychart " + str);

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
                if (arr.length >= NRITEMS) {
                    sampleTime = parseFloat(arr[0]) * 1000 + timeOffset;
                    arr.splice(0, 1); // remove timestamp (arr[0[]) from array
                    plot(chart, arr, sampleTime);
                }
            }
            chart.update();
        }
    }
}

function timer() {
    var arr;
    var str;

    if (SIMULATE) {
        simplot();
    }
    else {
        if (firstRequest) {
            clear();
            arr = getItem("getDayLogMeasValues");
            plotLog(dayChart, arr);
            arr = getItem("getHourLogMeasValues");
            plotLog(hourChart, arr);
            firstRequest = false;
        }
        str = getItem("getRTMeasValues");
        if (str) {
            arr = str.split(",");
            if (arr.length >= 3) {
                if (arr[1] > 0) {
                    if (arr[1] != lastTimeStamp) {
                        lastTimeStamp = arr[1];
                        plotLog(hourChart, str);
                        // average values for dayLog
                        var values = str.split(",");
                        for (var m = 0; m < NRITEMS; m++) {
                            averagedValues[m] += parseFloat(values[m + 1]); // values[0] is timestamp
                        }
                        nrAverages++;

                        dayLogPrescaler--;
                        if (dayLogPrescaler == 0) {
                            dayLogPrescaler = DAYLOGINTERVAL / REQUESTINTERVAL;
                            var str;
                            str = arr[0].toString() + ","; // timestamp
                            for (var m = 0; m < NRITEMS; m++) {
                                str += (averagedValues[m] / nrAverages).toString();
                                if (m < NRITEMS - 1)
                                    str += ",";
                                else
                                    str += "\n";
                            }
                            nrAverages = 0;
                            plotLog(dayChart, str);
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
function plotTest() {
    var value = [0, 0, 0, 0];
    for (let i = 0; i < 100; ++i) {
        for (let idx = 0; idx < 4; idx++) {
            value[idx] = Math.cos(i / 10) + idx;
        }
        var sec = Date.now();//  / 1000;  // mseconds since 1-1-1970 date
        plot(hourChart, value, sec + i * 10000);
        plot(dayChart, value, sec + i * 10000);
    }
    hourChart.update();
    dayChart.update();
}

var presc = 3;
var simVal = 0;
var simTs = 0;

function simplot() {

    var value;
    for (let idx = 0; idx < 100; idx++) {
        value= (simTs).toString() +',';

        for (let idx2 = 0; idx2 < 4; idx2++) {
            value = value + ((1 + Math.cos(simVal) + idx2).toString() +',');

        }
        value = value +'\n';
        simTs++;
        simVal +=0.02;
    }
    if ( presc-- == 0) {
        plotLog (dayChart,value);
        presc = 3;
    }

    plotLog (hourChart,value);

}