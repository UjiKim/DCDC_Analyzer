#include "core/BuckConverter.h"
#include <numbers>

std::complex<double> BuckConverter::getGd(double freq) {
    double s_val = 2.0 * std::numbers::pi * freq;
    std::complex<double> s(0, s_val);
    
    // 파라미터 로드 (PowerStageParams의 멤버 변수 활용)
    double Vin = m_params.vin();
    double Vout = m_params.vout();
    double D1 = m_params.d1();
    double M = m_params.M();
    double R = m_params.Rload();
    double L = m_params.L();
    double C = m_params.C();
    double Resr = m_params.rEsr();

    // ESR Zero는 공통 사항
    double wz = 1.0 / (Resr * C);

    if (m_params.mode() == OperationMode::CCM) {
        // --- [CCM: 2차 시스템 모델] ---
        double G0 = Vin;                 // DC Gain
        double w0 = 1.0 / std::sqrt(L * C); // 공진 주파수
        double Q = R * std::sqrt(C / L);    // 품질 계수

        std::complex<double> num = G0 * (1.0 + s / wz);
        std::complex<double> den = 1.0 + (s / (w0 * Q)) + (s * s / (w0 * w0));
        return num / den;

    } else {
        // --- [DCM: 1차 시스템 모델] ---
        // DC Gain: (2*Vout/D1) * ((1-M)/(2-M))
        double G0 = (2.0 * Vout / D1) * ((1.0 - M) / (2.0 - M));
        // Pole: ((2-M)/(1-M)) * (1/RC)
        double wp = ((2.0 - M) / (1.0 - M)) * (1.0 / (R * C));

        return G0 * (1.0 + s / wz) / (1.0 + s / wp);
    }
}

std::complex<double> BuckConverter::getGv(double freq) {
    double s_val = 2.0 * std::numbers::pi * freq;
    std::complex<double> s(0, s_val);
    
    double D1 = m_params.d1();
    double M = m_params.M();
    double R = m_params.Rload();
    double L = m_params.L();
    double C = m_params.C();
    double Resr = m_params.rEsr();
    double wz = 1.0 / (Resr * C);

    if (m_params.mode() == OperationMode::CCM) {
        // --- [CCM Gvg] ---
        double Gvg0 = D1;                // DC Gain
        double w0 = 1.0 / std::sqrt(L * C); 
        double Q = R * std::sqrt(C / L);

        std::complex<double> num = Gvg0 * (1.0 + s / wz);
        std::complex<double> den = 1.0 + (s / (w0 * Q)) + (s * s / (w0 * w0));
        return num / den;

    } else {
        // --- [DCM Gvg] ---
        double Gvg0 = M;                 // DC Gain
        double wp = ((2.0 - M) / (1.0 - M)) * (1.0 / (R * C)); // Pole은 Gd와 동일

        return Gvg0 * (1.0 + s / wz) / (1.0 + s / wp);
    }
}