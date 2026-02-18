#include "core/BoostConverter.h"
#include <numbers>
#include <cmath>

std::complex<double> BoostConverter::getGd(double freq) {
    double s_val = 2.0 * std::numbers::pi * freq;
    std::complex<double> s(0, s_val);
    
    // 파라미터 로드
    double Vin  = m_params.vin();
    double Vout = m_params.vout();
    double D1   = m_params.d1();
    double M    = m_params.M();
    double R    = m_params.Rload();
    double L    = m_params.L();
    double C    = m_params.C();
    double Resr = m_params.rEsr();

    // ESR Zero (Common)
    double wz_esr = 1.0 / (Resr * C);

    if (m_params.mode() == OperationMode::CCM) {
        // --- [CCM: 2차 시스템 + RHP Zero] ---
        // DC Gain: Vout / (1 - D1)
        double G0 = Vout / (1.0 - D1);
        // Resonant Frequency: (1 - D1) / sqrt(LC)
        double w0 = (1.0 - D1) / std::sqrt(L * C);
        // Q factor: (1 - D1) * R * sqrt(C / L)
        double Q = (1.0 - D1) * R * std::sqrt(C / L);
        // RHP Zero: R * (1 - D1)^2 / L
        double wrhz = (R * std::pow(1.0 - D1, 2)) / L;

        // Gvd(s) = G0 * (1 + s/wz_esr) * (1 - s/wrhz) / (1 + s/(w0*Q) + s^2/w0^2)
        std::complex<double> num = G0 * (1.0 + s / wz_esr) * (1.0 - s / wrhz); // RHP Zero는 분자에 마이너스(-) 항
        std::complex<double> den = 1.0 + (s / (w0 * Q)) + (s * s / (w0 * w0));
        return num / den;

    } else {
        // --- [DCM: 1차 시스템] ---
        // DC Gain: (2*Vout/D1) * ((M-1)/(2*M-1))
        double G0 = (2.0 * Vout / D1) * ((M - 1.0) / (2.0 * M - 1.0));
        // Pole: ((2*M-1)/(M-1)) * (1/RC)
        double wp = ((2.0 * M - 1.0) / (M - 1.0)) * (1.0 / (R * C));

        // Gvd(s) = G0 * (1 + s/wz_esr) / (1 + s/wp)
        return G0 * (1.0 + s / wz_esr) / (1.0 + s / wp);
    }
}

std::complex<double> BoostConverter::getGv(double freq) {
    double s_val = 2.0 * std::numbers::pi * freq;
    std::complex<double> s(0, s_val);
    
    double D1   = m_params.d1();
    double M    = m_params.M();
    double R    = m_params.Rload();
    double L    = m_params.L();
    double C    = m_params.C();
    double Resr = m_params.rEsr();
    double wz_esr = 1.0 / (Resr * C);

    if (m_params.mode() == OperationMode::CCM) {
        // --- [CCM Gvg] ---
        // DC Gain: 1 / (1 - D1)
        double Gvg0 = 1.0 / (1.0 - D1);
        double w0 = (1.0 - D1) / std::sqrt(L * C);
        double Q = (1.0 - D1) * R * std::sqrt(C / L);

        std::complex<double> num = Gvg0 * (1.0 + s / wz_esr);
        std::complex<double> den = 1.0 + (s / (w0 * Q)) + (s * s / (w0 * w0));
        return num / den;

    } else {
        // --- [DCM Gvg] ---
        // DC Gain: M
        double Gvg0 = M;
        double wp = ((2.0 * M - 1.0) / (M - 1.0)) * (1.0 / (R * C));

        return Gvg0 * (1.0 + s / wz_esr) / (1.0 + s / wp);
    }
}