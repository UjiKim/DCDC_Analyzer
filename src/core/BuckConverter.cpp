#include "core/BuckConverter.h"
#include <numbers>
#include <algorithm>

std::complex<double> BuckConverter::getGd(double freq) {
    double s_val = 2.0 * std::numbers::pi * freq;
    std::complex<double> s(0, s_val);
    
    double Vin = m_params.vin();
    double Vout = m_params.vout();
    double D1 = m_params.d1();
    double M = m_params.M();
    double R = std::max(1e-6, m_params.Rload()); // 부하 저항 0 방지
    double L = m_params.L();
    double C = m_params.C();
    double rL = m_params.rDcr();  // [반영] 인덕터 DCR
    double rC = m_params.rEsr();  // [반영] 커패시터 ESR

    // [버그 방지] ESR이 0일 경우 wz가 무한대가 되어 수치가 폭발하는 것을 방지
    double rC_safe = std::max(1e-9, rC);
    double wz = 1.0 / (rC_safe * C);

    if (m_params.mode() == OperationMode::CCM) {
        // --- [CCM: 2차 시스템 모델 + DCR/ESR 댐핑] ---
        double G0 = Vin; 
        double w0 = 1.0 / std::sqrt(L * C);
        
        // [정확한 수식] DCR과 ESR이 분모의 s항(댐핑)에 기여
        // den = 1 + s*(L/R + (rL+rC)*C) + s^2*LC
        std::complex<double> num = G0 * (1.0 + s / wz);
        std::complex<double> den = 1.0 + s * (L/R + (rL + rC)*C) + (s * s / (w0 * w0));
        return num / den;
    } else {
        // --- [DCM: 1차 시스템 모델] ---
        double G0 = (2.0 * Vout / D1) * ((1.0 - M) / (2.0 - M));
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