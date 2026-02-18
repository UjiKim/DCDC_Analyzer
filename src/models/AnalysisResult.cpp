#include "models/AnalysisResult.h"
#include <algorithm>
#include <limits>

void AnalysisResult::addPoint(double frequency, double magnitudeDb, double phaseDeg) {
    m_frequencies.push_back(frequency);
    m_magnitudes.push_back(magnitudeDb);
    m_phases.push_back(phaseDeg);
}

void AnalysisResult::clear() {
    m_frequencies.clear();
    m_magnitudes.clear();
    m_phases.clear();
}

double AnalysisResult::minMagnitude() const {
    if (m_magnitudes.empty()) return 0.0;
    return *std::min_element(m_magnitudes.begin(), m_magnitudes.end());
}

double AnalysisResult::maxMagnitude() const {
    if (m_magnitudes.empty()) return 0.0;
    return *std::max_element(m_magnitudes.begin(), m_magnitudes.end());
}

double AnalysisResult::minPhase() const {
    if (m_phases.empty()) return -180.0;
    return *std::min_element(m_phases.begin(), m_phases.end());
}

double AnalysisResult::maxPhase() const {
    if (m_phases.empty()) return 180.0;
    return *std::max_element(m_phases.begin(), m_phases.end());
}