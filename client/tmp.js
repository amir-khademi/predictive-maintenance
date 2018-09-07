/* Rickshaw high-frequency real time data visualization (OPTIMIZED)*/

let updateInterval = 200;
let offset = 0;
let api = 'http://31.184.132.36:8888/api/points?format=json&limit=800&offset=';

/* Rickshaw.js initialization */
let chart = new Rickshaw.Graph({
    element: document.querySelector("#demo_chart"),
    width: "1000",
    height: "200",
    renderer: "line",
    min: "0",
    max: "5000",
    series: new Rickshaw.Series.FixedDuration([{
        name: 'one',
        color: '#446CB3'
    }], undefined, {
        timeInterval: updateInterval,
        maxDataPoints: 10000
    })
});

let y_axis = new Rickshaw.Graph.Axis.Y({
    graph: chart,
    orientation: 'left',
    // tickFormat: function (y) {
    //     return y.toFixed(2);
    // },
    ticks: 5,
    element: document.getElementById('y_axis'),
});

setInterval(insertRandomDatapoints, updateInterval);

/* Function that generates and inserts five random data points into the chart */
function insertRandomDatapoints() {
    // for (let i = 0; i < 400; i++) {
    //     let tmpData = {
    //         one: Math.floor(Math.random() * 2000) + 10
    //     };
    //     chart.series.addData(tmpData);
    // }
    // chart.render();


    let url = api.concat(offset);
    fetch(url).then(response => {
        return response.json();
    }).then(data => {
        for (point of data.results) {
            let tmpData = {
                one: point.value
            };
            chart.series.addData(tmpData);
        }
        chart.render();
        offset += 800;
    }).catch(err => {
        // Do something for an error here
    });
}