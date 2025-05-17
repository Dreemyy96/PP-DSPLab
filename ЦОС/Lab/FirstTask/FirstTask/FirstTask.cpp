#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include "Harmonic.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

vector<Harmonic> readHarmonicsFromUser(int& samplingRate, int& numPoints) {
    int numHarmonics;
    cout << "Enter the number of harmonics: ";
    cin >> numHarmonics;

    vector<Harmonic> harmonics(numHarmonics);
    for (int i = 0; i < numHarmonics; ++i) {
        double amplitude;
        cout << "Harmonic amplitude " << (i + 1) << ": ";
        cin >> amplitude;
        harmonics[i].amplitude = amplitude;
        harmonics[i].frequency = 50.0 * (i + 1); 
        harmonics[i].phaseShift = 0.0; 
    }

    cout << "Enter the sampling frequency (Hz): ";
    cin >> samplingRate;

    cout << "Enter the number of points:";
    cin >> numPoints;

    return harmonics;
}

vector<Harmonic> readHarmonicsFromFile(const string& filename, int& samplingRate, int& numPoints) {
    ifstream file(filename);
    vector<Harmonic> harmonics;
    if (!file) {
        cerr << "Couldn't open the file!" << endl;
        return harmonics;
    }

    int numHarmonics;
    file >> numHarmonics;
    harmonics.resize(numHarmonics);

    for (int i = 0; i < numHarmonics; ++i) {
        file >> harmonics[i].amplitude >> harmonics[i].frequency >> harmonics[i].phaseShift;
    }

    file >> samplingRate >> numPoints;

    file.close();
    return harmonics;
}

double computeDeltaT(int samplingRate) {
    return 1.0 / samplingRate;
}

vector<double> generateSignal(const vector<Harmonic>& harmonics, int numPoints, double deltaT) {
    vector<double> signal(numPoints, 0.0);

    for (int n = 0; n < numPoints; ++n) {
        double t = n * deltaT;
        double value = 0.0;

        for (const auto& h : harmonics) {
            value += h.amplitude * sin(2 * M_PI * h.frequency * t + h.phaseShift);
        }

        signal[n] = value;
    }

    return signal;
}

double computeIMax(const vector<double>& signal) {
    double maxAmp = 0.0;
    for (double val : signal) {
        if (abs(val) > maxAmp) {
            maxAmp = abs(val);
        }
    }

    return ceil(maxAmp * 1.1);
}

vector<int> quantizeSignal(const vector<double>& signal, double imax, int numLevels) {
    vector<int> digital(signal.size());

    double step = (2 * imax) / numLevels;
    for (size_t i = 0; i < signal.size(); ++i) {
        int level = static_cast<int>((signal[i] + imax) / step);
        if (level < 0) level = 0;
        if (level >= numLevels) level = numLevels - 1;
        digital[i] = level;
    }

    return digital;
}

void exportToCSV(const vector<double>& x, const vector<double>& analog, const vector<int>& digital, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Couldn't open the file for writing: " << filename << endl;
        return;
    }

    file << "time,analog,digital\n";
    for (size_t i = 0; i < x.size(); ++i) {
        file << x[i] << "," << analog[i] << "," << digital[i] << "\n";
    }

    file.close();
    cout << "The data is exported to " << filename << endl;
}

int main() {
    vector<Harmonic> harmonics;
    int samplingRate = 0;
    int numPoints = 0;

    cout << "Select the input method (1 - from the keyboard, 2 - from the file): ";
    int choice;
    cin >> choice;

    if (choice == 1) {
        harmonics = readHarmonicsFromUser(samplingRate, numPoints);
    }
    else {
        string filename;
        cout << "Enter the file name: ";
        cin >> filename;
        harmonics = readHarmonicsFromFile(filename, samplingRate, numPoints);
    }

    double deltaT = computeDeltaT(samplingRate);
    auto signal = generateSignal(harmonics, numPoints, deltaT);

    cout << "The generation is completed. An example of the first 5 signal points:" << endl;
    for (int i = 0; i < 5 && i < signal.size(); ++i) {
        cout << "y[" << i << "] = " << signal[i] << endl;
    }

    double imax = computeIMax(signal);
    cout << "imax = " << imax << endl;

    int numLevels = 16; 
    auto digitalSignal = quantizeSignal(signal, imax, numLevels);

    cout << "Digital signal (first 5 levels):" << endl;
    for (int i = 0; i < 5 && i < digitalSignal.size(); ++i) {
        cout << "q[" << i << "] = " << digitalSignal[i] << endl;
    }

    vector<double> x(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        x[i] = i * deltaT;
    }

    exportToCSV(x, signal, digitalSignal, "output.csv");


    return 0;
}