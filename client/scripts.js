// Variable Initialization
let api = 'http://31.184.132.12:8888/api/points';
let points = [];
let offset = 0;
let timeout = 100;
let window_size = 300;

// CanvasJS chart initialize
let chart = new CanvasJS.Chart('chart_container', {
    data: [{
        type: "line",
        dataPoints: points,
        // xValueType: "dateTime",
    }],
    axisX: [{
        tickLength: 0,
        lineThickness: 0,
        margin: -5,
        labelFormatter: function (e) {
            return "";
        },
    }, {
        // viewportMinimum: 0,
        // viewportMaximum: 5,
        // interval: 1,
    }],
    axisY: {
        minimum: 1400,
        maximum: 2000
    },
});

// Get the count of points in the database
// to make it the start point for plotting the chart
const getPointsCount = () => {
    fetch(createURL(0, 1)).then(response => {
        return response.json();
    }).then(data => {
        offset = data.count;
        wait(timeout);
    }).catch(err => {
        console.log(err)
    });
};


// Fetch data from database periodically, process them and plot the chart
const fetchDataAndPlot = () => {
    fetch(createURL(offset, 1000000)).then(response => {
        return response.json();
    }).then(data => {
        for (point of data.results) {
            // points.push({
            //     x: new Date(point.datetime),
            //     y: parseInt(point.value)
            // });
            points.push({
                x: parseInt(point.id),
                y: parseInt(point.value)
            });
            if (points.length > window_size) {
                points.shift();
            }
        }
        chart.render();
        offset += data.results.length;
        wait(timeout);
    }).catch(err => {
        console.log(err)
    })
};


// creates a complete API URL
const createURL = (offset, limit) => {
    return api + '?offset=' + offset + '&limit=' + limit;
};


// wait for a specific duration
const wait = (duration) => {
    setTimeout(fetchDataAndPlot, duration)
};

getPointsCount();