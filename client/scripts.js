let api = 'http://31.184.132.36:8888/api/points';
let points = [];
let offset = 0;

// for (let i = 0; i < 20; i++) {
//     points.push([new Date('2009/07/12'), i]);
// }

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

// /* Rickshaw.js initialization */
// let chart = new Rickshaw.Graph({
//     element: document.querySelector("#chart_container"),
//     width: "1000",
//     height: "200",
//     renderer: "line",
//     min: "0",
//     max: "4096",
//     series: new Rickshaw.Series.FixedDuration([{
//         name: 'one',
//         color: '#446CB3'
//     }], undefined, {
//         timeInterval: 1000,
//         maxDataPoints: 20000
//     })
// });

// getData();

fetch(api + '?offset=' + offset + '&limit=1').then(response => {
    return response.json();
}).then(data => {
    offset = data.count;
    dummy();
    // alert(offset);
    // setInterval(
    //     fetch(api + '?offset=' + offset + '&limit=2000').then(response => {
    //         return response.json();
    //     }).then(data => {
    //         // process data
    //         for (point of data.results) {
    //             points.push({
    //                 x: point.datetime,
    //                 y: point.value
    //             });
    //             points.shift()
    //         }
    //         chart.render();
    //         offset += 2000;
    //     }).catch(err => {
    //         console.log(err)
    //     })
    //     , 1000)
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
            // process data
            if (points.length > 1000) {
                points.shift();
            }
            // let tmpData = {
            //     one: point.value
            // };
            // chart.series.addData(tmpData);
        }
        chart.render();
        offset += data.results.length;
        // offset += 2000;
        dummy();
    }).catch(err => {
        console.log(err)
    })
}

function getData() {
    fetch(api + '?offset=' + offset + '&limit=1000').then(response => {
        return response.json();
    }).then(data => {
        for (point of data.results) {
            points.push([
                new Date(point.datetime),
                point.value
            ]);
            if (points.length > 2000) {
                points.shift();
            }
        }
        chart.updateOptions({'file': points});
        offset += 2000;
        dummy();
    }).catch(err => {
        console.log(err)
    })
}

function dummy() {
    setTimeout(fetchData, 200)
}

// console.log(points);
// let chart = new Dygraph(document.getElementById("chart_container"), points,
//     {
//         labels: ["x", "y"]
//     });