#ifndef ANALYSISRESULT_H
#define ANALYSISRESULT_H

#include <vector>

/**
 * @brief AC 분석 결과를 저장하고 관리하는 클래스
 */
class AnalysisResult {
public:
    AnalysisResult() = default;

    // --- [데이터 추가 및 초기화] ---
    void addPoint(double frequency, double magnitudeDb, double phaseDeg);
    void clear();

    // --- [Getters] ---
    const std::vector<double>& frequencies() const { return m_frequencies; }
    const std::vector<double>& magnitudes() const { return m_magnitudes; }
    const std::vector<double>& phases() const { return m_phases; }

    size_t count() const { return m_frequencies.size(); }
    bool isEmpty() const { return m_frequencies.empty(); }

    // --- [통계 유틸리티 (UI에서 축 범위를 잡을 때 유용)] ---
    double minMagnitude() const;
    double maxMagnitude() const;
    double minPhase() const;
    double maxPhase() const;

private:
    std::vector<double> m_frequencies;
    std::vector<double> m_magnitudes;
    std::vector<double> m_phases;
};

#endif // ANALYSISRESULT_H