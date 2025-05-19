#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <cmath>
#include <fftw3.h>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

struct SignalPoint {
    double time;
    double analog;
    int digital;
};

vector<SignalPoint> readCSV(const string& filename) {
    vector<SignalPoint> data;
    ifstream file(filename);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string timeStr, analogStr, digitalStr;
        getline(ss, timeStr, ',');
        getline(ss, analogStr, ',');
        getline(ss, digitalStr, ',');
        SignalPoint point;
        point.time = stod(timeStr);
        point.analog = stod(analogStr);
        point.digital = stoi(digitalStr);
        data.push_back(point);
    }
    return data;
}

vector<double> dequantizeSignal(const vector<int>& digital, double imax, int numLevels) {
    vector<double> dequantize;
    dequantize.reserve(digital.size());
    double step = (2 * imax) / numLevels; 
    for (int q : digital) {
        double value = (q + 0.5) * step - imax;
        dequantize.push_back(value);
    }
    return dequantize;
}

void computeFFT(const vector<double>& signal, vector<complex<double>>& spectrum) {
    int N = signal.size();
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    for (int i = 0; i < N; ++i) {
        in[i][0] = signal[i];
        in[i][1] = 0.0;
    }

    fftw_plan p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    spectrum.resize(N);
    for (int i = 0; i < N; ++i) {
        spectrum[i] = complex<double>(out[i][0], out[i][1]);
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

vector<double> inverseFFT(const vector<complex<double>>& spectrum) {
    int N = spectrum.size();
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    for (int i = 0; i < N; ++i) {
        in[i][0] = spectrum[i].real();
        in[i][1] = spectrum[i].imag();
    }

    fftw_plan p = fftw_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    vector<double> restored(N);
    for (int i = 0; i < N; ++i) {
        restored[i] = out[i][0] / N;
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    return restored;
}

double computeTimeDomainError(const vector<double>& original, const vector<double>& restored) {
    if (original.size() != restored.size() || original.empty()) return -1.0;

    double errorSum = 0.0;
    int count = 0;

    for (size_t i = 0; i < original.size(); ++i) {
        if (fabs(original[i]) > 1e-9) {
            errorSum += fabs((restored[i] - original[i]) / original[i]);
            ++count;
        }
    }

    return (count > 0) ? (errorSum / count) * 100.0 : -1.0;
}

double computeIMax(const vector<double>& analogSpectrum) {
    double maxAmp = 0.0;
    for (double val : analogSpectrum) {
        if (abs(val) > maxAmp) {
            maxAmp = abs(val);
        }
    }

    return ceil(maxAmp * 1.1);
}

void saveSpectrumToCSV(const string& filename, const vector<double>& analogSpec, const vector<double>& digitalSpec) {
    ofstream out(filename);
    out << "index,analog,restored\n";
    int N = min(analogSpec.size(), digitalSpec.size());
    for (int i = 0; i < N; ++i) {
        out << i << "," << analogSpec[i] << "," << digitalSpec[i] << "\n";
    }
}

int main() {
    string filename = "output.csv";
    int numLevels = 16;

    vector<SignalPoint> data = readCSV(filename);

    vector<double> analog;
    vector<int> quantizeSignal;
    for (const auto& point : data) {
        analog.push_back(point.analog);
        quantizeSignal.push_back(point.digital);
    }

    double imax = computeIMax(analog);

    vector<double> restoredSignal = dequantizeSignal(quantizeSignal, imax, numLevels);

    vector<complex<double>> spectrum;
    computeFFT(restoredSignal, spectrum);
    vector<double> reconstructed = inverseFFT(spectrum);

    double errorPercent = computeTimeDomainError(analog, reconstructed);

    cout << fixed << setprecision(2);
    cout << "Погрешность между спектрами: " << errorPercent << "%" << endl;

    if (errorPercent > 5.0) {
        cout << "Погрешность превышает допустимые 5%!" << endl;
    }
    else {
        cout << "Погрешность в допустимых пределах." << endl;
    }

    saveSpectrumToCSV("fft_results.csv", analog, reconstructed);

    return 0;
}
