var hourChart;
var dayChart;
var mcLabelTxt = ["PM1", "PM2.5", "PM4", "PM10"];
var NRITEMS = 4;
var MAXPOINTS = 50;


var firstTime = true;
var INFOTABLENAME = "sensorInfoTable";
var REQUESTINTERVAL = 1;  // sec
var DAYLOGINTERVAL = (5 * 60); // min
var lastTimeStamp = 0;
var dayLogPrescaler = 1;
var firstRequest = true;


const Utils = ChartUtils.init();

const DATA_COUNT = 7
const NUMBER_CFG = { count: MAXPOINTS, min: 0, max: 2000 }


function initChart() {
    window.addEventListener('DOMContentLoaded', function () {
        var ctx = document.getElementById("hourChart");
        hourChart = new Chart(ctx, config);
        var ctx = document.getElementById("dayChart");
        dayChart = new Chart(ctx, config);
       // plotTest();
        timer();
        startTimer();
    });
};


const data = {
    datasets: [
        {
            label: 'PM1',
            data: Utils.numbers(NUMBER_CFG),
            borderColor: Utils.CHART_COLORS.red,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.red, 0.5),
            borderWidth: 2,
            borderRadius: Number.MAX_VALUE,
            borderSkipped: false
        },
        {
            label: 'PM2.5',
            data: Utils.numbers(NUMBER_CFG),
            borderColor: Utils.CHART_COLORS.yellow,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.orange, 0.5),
            borderWidth: 2,
            borderRadius: 5,
            borderSkipped: false
        },
        {
            label: 'PM4',
            data: Utils.numbers(NUMBER_CFG),
            borderColor: Utils.CHART_COLORS.blue,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.blue, 0.5),
            borderWidth: 2,
            borderRadius: 5,
            borderSkipped: false
        },
        {
            label: 'PM10',
            data: Utils.numbers(NUMBER_CFG),
            borderColor: Utils.CHART_COLORS.green,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.green, 0.5),
            borderWidth: 2,
            borderRadius: 5,
            borderSkipped: false
        }
    ]
}

const config = {
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
                text: 'Chart.js Bar Chart'
            }
        }
    }
}

function plotTest() {
    var value = [0, 0, 0, 0];
    for (let i = 0; i < 100; ++i) {
        for (let idx = 0; idx < 4; idx++) {
            value[idx] = Math.cos(i / 20) + idx;
        }
        var sec = Date.now();//  / 1000;  // mseconds since 1-1-1970 date
        plot(hourChart, value, sec + i * 10000);
        plot(datChart, value, sec + i * 10000);
    }
    hourChart.update();
    dayChart.update();
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
    setInterval(function () { timer() }, REQUESTINTERVAL * 1000);
}
function forgetWifi() {
    getItem("forgetWifi");
}

// plots one sample ( array of 4 items)
function plot(chart, values, timeStamp) {
    var row;
    const data = chart.data;
    if (data.datasets.length > 0) {

        for (let index = 0; index < data.datasets.length; ++index) {
            var value = parseFloat(values[index]);
            data.datasets[index].data.push(value);
        }

        var date = new Date(timeStamp);
        var labelText = date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
        data.labels.push(labelText);

        if (data.labels.length > MAXPOINTS) {
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
        //	simplot();
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
                                if (m < mcLabelTxt.length - 1)
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
