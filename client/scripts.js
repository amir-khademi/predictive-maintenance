let api = 'http://31.184.132.36:8888/api/points';
let points = [];
let offset = 0;

// CanvasJS
let chart = new CanvasJS.Chart('chart_container', {
    data: [{
        type: "line",
        dataPoints: points,
        // xValueType: "dateTime",
    }],
    axisX: [{
        tickLength: 0,
        lineThickness: 0,
        labelFormatter: function (e) {
            return "";
        },
        margin: -5
    }, {
        // viewportMinimum: 0,
        // viewportMaximum: 5,
        // interval: 1,
    }],
    axisY: {
        minimum: 0,
        maximum: 4096
    },
});

fetch(api + '?offset=' + offset + '&limit=1').then(response => {
    return response.json();
}).then(data => {
    offset = data.count;
    dummy();
}).catch(err => {
    console.log(err)
});

function fetchData() {
    fetch(api + '?offset=' + offset + '&limit=1000000').then(response => {
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
            if (points.length > 1000) {
                points.shift();
            }
        }
        chart.render();
        offset += data.results.length;
        // offset += 2000;
        dummy();
    }).catch(err => {
        console.log(err)
    })
}

function dummy() {
    setTimeout(fetchData, 200)
}