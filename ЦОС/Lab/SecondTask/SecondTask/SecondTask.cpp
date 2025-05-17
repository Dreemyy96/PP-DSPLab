#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <fftw3.h>

using namespace std;

bool readCSV(const string& filename, vector<double>& time, vector<double>& analog, vector<int>& digital) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Couldn't open file " << filename << endl;
        return false;
    }

    string line;
    getline(file, line); 

    while (getline(file, line)) {
        stringstream ss(line);
        double t, a;
        int d;
        char comma;

        if (ss >> t >> comma >> a >> comma >> d) {
            time.push_back(t);
            analog.push_back(a);
            digital.push_back(d);
        }
    }

    file.close();
    return true;
}

vector<double> normalizeDigital(const vector<int>& digital, int numLevels, double imax) {
    vector<double> result(digital.size());
    double step = (2.0 * imax) / numLevels;

    for (size_t i = 0; i < digital.size(); ++i) {
        result[i] = digital[i] * step - imax + step / 2.0;
    }

    return result;
}

double averageRelativeError(const vector<double>& original, const vector<double>& restored) {
    if (original.size() != restored.size() || original.empty()) return -1.0;

    double errorSum = 0.0;
    int count = 0;
    const double threshold = 1e-2; // порог амплитуды

    for (size_t i = 0; i < original.size(); ++i) {
        if (fabs(original[i]) > threshold) {
            errorSum += fabs((restored[i] - original[i]) / original[i]);
            ++count;
        }
    }

    return (count > 0) ? (errorSum / count) * 100.0 : -1.0;
}


void exportCSV(const string& filename, const vector<double>& time, const vector<double>& analog, const vector<double>& restored) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Couldn't open file for writing: " << filename << endl;
        return;
    }

    file << "time,analog,restored\n";
    for (size_t i = 0; i < time.size(); ++i) {
        file << time[i] << "," << analog[i] << "," << restored[i] << "\n";
    }

    file.close();
    cout << "Restored signal exported to " << filename << endl;
}

int main() {
    const string inputFilename = "output.csv";
    const string outputFilename = "restored.csv";
    const int numLevels = 16;
    const double imax = 16.0;

    vector<double> time;
    vector<double> analog;
    vector<int> digital;

    if (!readCSV(inputFilename, time, analog, digital)) {
        return 1;
    }

    if (analog.size() != digital.size() || analog.empty()) {
        cerr << "Data size mismatch or empty data" << endl;
        return 1;
    }

    vector<double> digitalNormalized = normalizeDigital(digital, numLevels, imax);
    size_t N = digitalNormalized.size();

    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    for (size_t i = 0; i < N; ++i) {
        in[i][0] = digitalNormalized[i];
        in[i][1] = 0.0;
    }

    fftw_plan plan_forward = fftw_plan_dft_1d((int)N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan_forward);

    fftw_plan plan_backward = fftw_plan_dft_1d((int)N, out, in, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan_backward);

    vector<double> restoredSignal(N);
    for (size_t i = 0; i < N; ++i) {
        restoredSignal[i] = in[i][0] / N;
    }

    double avgRelError = averageRelativeError(analog, restoredSignal);
    cout << "Average relative error: " << avgRelError << " %" << endl;
    if (avgRelError <= 5.0) {
        cout << "? Error is within threshold!" << endl;
    }
    else {
        cout << "? Error exceeds threshold!" << endl;
    }

    exportCSV(outputFilename, time, analog, restoredSignal);

    fftw_destroy_plan(plan_forward);
    fftw_destroy_plan(plan_backward);
    fftw_free(in);
    fftw_free(out);

    return 0;
}
