// Variable Initialization
let api = 'http://31.184.132.12:8888/api/points';
let points = [];
let offset = 0;
let timeout = 1000;
let dataLength = 0;
let sampling_rate = 2500;
let minY = -1;
let maxY = 2;
let signal = [];
let temp_signal = [];

// CanvasJS chart initialize
let chart = new CanvasJS.Chart('chart_container', {
    title: {
        text: "Spectrum (Domain-Frequency)"
    },
    data: [{
        type: "line",
        dataPoints: points,
    }],
    axisX: [{
        labelFormatter: function (e) {
            return parseInt((e.value * sampling_rate) / dataLength); // converting x axis to frequency
            // return e.value; // default
        },
    }],
    axisY: {
        minimum: minY,
        maximum: maxY
    },
});


// Get the count of points in the database
// to make it the start point for plotting the chart (real-time)
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
        dataLength = data.results.length;
        // empty the variables for the next round
        signal = [];
        signal.length = 0;
        temp_signal = [];
        temp_signal.length = 0;

        // getting only values from the results to create a signal
        for (point of data.results) {
            temp_signal.push(point.value)
        }

        // calculate the average of signal
        let avg = temp_signal.reduce(function (p, c, i, a) {
            return p + (c / a.length)
        }, 0);

        // removing the DC component by minus the average from all the signal members
        for (point of data.results) {
            signal.push(point.value - avg)
        }

        // calculate the FFT
        // credit : https://github.com/dntj/jsfft
        let fft = new ComplexArray(signal);
        fft.FFT();
        fft.forEach((value, i) => {
            points.push({
                x: i,
                // calculating Amplitude-Units-Peak (Apk) factor for Y axis
                // read more here : https://sooeet.com/math/online-fft-calculator.php
                y: ((Math.sqrt(Math.pow(value.imag, 2) + Math.pow(value.real, 2))) / (data.results.length)) * 2
            });
        });
        chart.render();

        // empty the chart for the next round
        for (let j = 0; j < 50000; j++) {
            points.shift()
        }

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


////////////////////////////////////////////////////////////////////////////////
//              FFT Classes and Functions
//         credit : https://github.com/dntj/jsfft
////////////////////////////////////////////////////////////////////////////////
class baseComplexArray {
    constructor(other, arrayType = Float32Array) {
        if (other instanceof ComplexArray) {
            // Copy constuctor.
            this.ArrayType = other.ArrayType;
            this.real = new this.ArrayType(other.real);
            this.imag = new this.ArrayType(other.imag);
        } else {
            this.ArrayType = arrayType;
            // other can be either an array or a number.
            this.real = new this.ArrayType(other);
            this.imag = new this.ArrayType(this.real.length);
        }

        this.length = this.real.length;
    }

    toString() {
        const components = [];

        this.forEach((value, i) => {
            components.push(
                `(${value.real.toFixed(2)}, ${value.imag.toFixed(2)})`
            );
        });

        return `[${components.join(', ')}]`;
    }

    forEach(iterator) {
        const n = this.length;
        // For gc efficiency, re-use a single object in the iterator.
        const value = Object.seal(Object.defineProperties({}, {
            real: {writable: true}, imag: {writable: true},
        }));

        for (let i = 0; i < n; i++) {
            value.real = this.real[i];
            value.imag = this.imag[i];
            iterator(value, i, n);
        }
    }

    // In-place mapper.
    map(mapper) {
        this.forEach((value, i, n) => {
            mapper(value, i, n);
            this.real[i] = value.real;
            this.imag[i] = value.imag;
        });

        return this;
    }

    conjugate() {
        return new ComplexArray(this).map((value) => {
            value.imag *= -1;
        });
    }

    magnitude() {
        const mags = new this.ArrayType(this.length);

        this.forEach((value, i) => {
            mags[i] = Math.sqrt(value.real * value.real + value.imag * value.imag);
        })

        return mags;
    }
}

const PI = Math.PI;
const SQRT1_2 = Math.SQRT1_2;

function FFT(input) {
    return ensureComplexArray(input).FFT();
};

function InvFFT(input) {
    return ensureComplexArray(input).InvFFT();
};

function frequencyMap(input, filterer) {
    return ensureComplexArray(input).frequencyMap(filterer);
};

class ComplexArray extends baseComplexArray {
    FFT() {
        return fft(this, false);
    }

    InvFFT() {
        return fft(this, true);
    }

    // Applies a frequency-space filter to input, and returns the real-space
    // filtered input.
    // filterer accepts freq, i, n and modifies freq.real and freq.imag.
    frequencyMap(filterer) {
        return this.FFT().map(filterer).InvFFT();
    }
}

function ensureComplexArray(input) {
    return input instanceof ComplexArray && input || new ComplexArray(input);
}

function fft(input, inverse) {
    const n = input.length;

    if (n & (n - 1)) {
        return FFT_Recursive(input, inverse);
    } else {
        return FFT_2_Iterative(input, inverse);
    }
}

function FFT_Recursive(input, inverse) {
    const n = input.length;

    if (n === 1) {
        return input;
    }

    const output = new ComplexArray(n, input.ArrayType);

    // Use the lowest odd factor, so we are able to use FFT_2_Iterative in the
    // recursive transforms optimally.
    const p = LowestOddFactor(n);
    const m = n / p;
    const normalisation = 1 / Math.sqrt(p);
    let recursive_result = new ComplexArray(m, input.ArrayType);

    // Loops go like O(n Σ p_i), where p_i are the prime factors of n.
    // for a power of a prime, p, this reduces to O(n p log_p n)
    for (let j = 0; j < p; j++) {
        for (let i = 0; i < m; i++) {
            recursive_result.real[i] = input.real[i * p + j];
            recursive_result.imag[i] = input.imag[i * p + j];
        }
        // Don't go deeper unless necessary to save allocs.
        if (m > 1) {
            recursive_result = fft(recursive_result, inverse);
        }

        const del_f_r = Math.cos(2 * PI * j / n);
        const del_f_i = (inverse ? -1 : 1) * Math.sin(2 * PI * j / n);
        let f_r = 1;
        let f_i = 0;

        for (let i = 0; i < n; i++) {
            const _real = recursive_result.real[i % m];
            const _imag = recursive_result.imag[i % m];

            output.real[i] += f_r * _real - f_i * _imag;
            output.imag[i] += f_r * _imag + f_i * _real;

            [f_r, f_i] = [
                f_r * del_f_r - f_i * del_f_i,
                f_i = f_r * del_f_i + f_i * del_f_r,
            ];
        }
    }

    // Copy back to input to match FFT_2_Iterative in-placeness
    // TODO: faster way of making this in-place?
    for (let i = 0; i < n; i++) {
        input.real[i] = normalisation * output.real[i];
        input.imag[i] = normalisation * output.imag[i];
    }

    return input;
}

function FFT_2_Iterative(input, inverse) {
    const n = input.length;

    const output = BitReverseComplexArray(input);
    const output_r = output.real;
    const output_i = output.imag;
    // Loops go like O(n log n):
    //   width ~ log n; i,j ~ n
    let width = 1;
    while (width < n) {
        const del_f_r = Math.cos(PI / width);
        const del_f_i = (inverse ? -1 : 1) * Math.sin(PI / width);
        for (let i = 0; i < n / (2 * width); i++) {
            let f_r = 1;
            let f_i = 0;
            for (let j = 0; j < width; j++) {
                const l_index = 2 * i * width + j;
                const r_index = l_index + width;

                const left_r = output_r[l_index];
                const left_i = output_i[l_index];
                const right_r = f_r * output_r[r_index] - f_i * output_i[r_index];
                const right_i = f_i * output_r[r_index] + f_r * output_i[r_index];

                output_r[l_index] = SQRT1_2 * (left_r + right_r);
                output_i[l_index] = SQRT1_2 * (left_i + right_i);
                output_r[r_index] = SQRT1_2 * (left_r - right_r);
                output_i[r_index] = SQRT1_2 * (left_i - right_i);

                [f_r, f_i] = [
                    f_r * del_f_r - f_i * del_f_i,
                    f_r * del_f_i + f_i * del_f_r,
                ];
            }
        }
        width <<= 1;
    }

    return output;
}

function BitReverseIndex(index, n) {
    let bitreversed_index = 0;

    while (n > 1) {
        bitreversed_index <<= 1;
        bitreversed_index += index & 1;
        index >>= 1;
        n >>= 1;
    }
    return bitreversed_index;
}

function BitReverseComplexArray(array) {
    const n = array.length;
    const flips = new Set();

    for (let i = 0; i < n; i++) {
        const r_i = BitReverseIndex(i, n);

        if (flips.has(i)) continue;

        [array.real[i], array.real[r_i]] = [array.real[r_i], array.real[i]];
        [array.imag[i], array.imag[r_i]] = [array.imag[r_i], array.imag[i]];

        flips.add(r_i);
    }

    return array;
}

function LowestOddFactor(n) {
    const sqrt_n = Math.sqrt(n);
    let factor = 3;

    while (factor <= sqrt_n) {
        if (n % factor === 0) return factor;
        factor += 2;
    }
    return n;
}