#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <complex>
#include <vector>
#include <QString>

namespace MathUtils {
    // 1. 복소수 -> Magnitude (dB) 변환: 20*log10(|G|)
    double toDb(const std::complex<double>& val);

    // 2. 복소수 -> Phase (Degree) 변환: arg(G) * 180/pi
    double toPhase(const std::complex<double>& val);

    // 3. 로그 스케일 주파수 배열 생성 (추후 구현)
    std::vector<double> generateLogSpace(double startFreq, double endFreq, int pointsPerDecade);
    // --- [추가] 4. 숫자를 SI 접두사로 변환 (예: 1000 -> 1k) ---
    QString formatSi(double value, const QString& unit = "");
}

#endif // MATHUTILS_H