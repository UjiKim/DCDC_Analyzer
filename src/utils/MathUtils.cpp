#include <cmath>
#include <numbers> // C++20: std::numbers::pi 활용
#include <vector>
#include "utils/MathUtils.h"

namespace MathUtils {

double toDb(const std::complex<double>& val) {
    // std::abs는 복소수의 크기(Magnitude)를 반환합니다.
    double magnitude = std::abs(val);
    
    // 로그 연산 시 0이 들어가지 않도록 매우 작은 값으로 하한 설정
    if (magnitude < 1e-15) return -300.0; 
    
    return 20.0 * std::log10(magnitude);
}

double toPhase(const std::complex<double>& val) {
    // std::arg는 복소수의 위상(Radian)을 -pi ~ pi 범위로 반환합니다.
    double phaseRad = std::arg(val);
    
    // Radian -> Degree 변환
    double phaseDeg = phaseRad * (180.0 / std::numbers::pi);
    
    return phaseDeg;
}

std::vector<double> generateLogSpace(double startFreq, double endFreq, int pointsPerDecade) {
    std::vector<double> frequencies;
    
    // 비정상적인 입력 방어 로직
    if (startFreq <= 0 || endFreq <= startFreq || pointsPerDecade <= 0) {
        return frequencies;
    }

    // 1. 로그 스케일의 시작과 끝점 계산 (예: 10 -> 1, 1,000,000 -> 6)
    double logStart = std::log10(startFreq);
    double logEnd = std::log10(endFreq);
    
    // 2. 전체 Decade 수와 총 데이터 포인트 계산
    double numDecades = logEnd - logStart;
    int totalPoints = static_cast<int>(numDecades * pointsPerDecade) + 1;
    
    frequencies.reserve(totalPoints);

    // 3. 로그 공간에서 균등하게 분할하여 주파수 생성
    for (int i = 0; i < totalPoints; ++i) {
        double currentLogVal = logStart + (static_cast<double>(i) / (totalPoints - 1)) * (logEnd - logStart);
        frequencies.push_back(std::pow(10.0, currentLogVal));
    }

    return frequencies;
}

// --- [추가] SI 접두사 변환 구현 ---
QString formatSi(double value, const QString& unit) {
    double absVal = std::abs(value);
    QString prefix = "";
    double scaled = value;

    if (absVal >= 1e6)      { scaled /= 1e6; prefix = "M"; }
    else if (absVal >= 1e3) { scaled /= 1e3; prefix = "k"; }
    else if (absVal >= 1.0) { /* 기본 단위 */ }
    else if (absVal >= 1e-3) { scaled *= 1e3; prefix = "m"; }
    else if (absVal >= 1e-6) { scaled *= 1e6; prefix = "u"; }
    else if (absVal >= 1e-9) { scaled *= 1e-9; prefix = "n"; }

    // 소수점 3자리까지 표기 (예: 1.234k)
    return QString::number(scaled, 'f', 3) + prefix + unit;
}

}