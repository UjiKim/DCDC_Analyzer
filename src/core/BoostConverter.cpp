#include "core/BoostConverter.h"
#include <numbers>
#include <cmath>
#include <algorithm>

std::complex<double> BoostConverter::getGd(double freq) {
    double s_val = 2.0 * std::numbers::pi * freq;
    std::complex<double> s(0, s_val);
    
    double Vin  = m_params.vin();
    double Vout = m_params.vout();
    double D1   = m_params.d1();
    double M    = m_params.M();
    double R    = std::max(1e-6, m_params.Rload());
    double L    = m_params.L();
    double C    = m_params.C();
    double rL   = m_params.rDcr(); 
    double rC   = m_params.rEsr();

    double rC_safe = std::max(1e-9, rC);
    double wz_esr = 1.0 / (rC_safe * C);

    if (m_params.mode() == OperationMode::CCM) {
        // --- [CCM: DCR 반영] ---
        double D_prime = 1.0 - D1;
        double Reff = R * std::pow(D_prime, 2);
        
        // [반영] DCR에 의한 DC Gain 감소율
        double Gain_dcr = 1.0 / (1.0 + rL / Reff);
        double G0 = (Vout / D_prime) * Gain_dcr;
        
        double w0 = D_prime / std::sqrt(L * C);
        double wrhz = Reff / L; // RHP Zero

        std::complex<double> num = G0 * (1.0 + s / wz_esr) * (1.0 - s / wrhz);
        // [반영] DCR/ESR 댐핑 성분 포함
        std::complex<double> den = 1.0 + s * (L/Reff + (rL + rC)*C) + (s * s / (w0 * w0));
        return num / den;

    } else {
        // --- [DCM] ---
        double G0 = (2.0 * Vout / D1) * ((M - 1.0) / (2.0 * M - 1.0));
        double wp = ((2.0 * M - 1.0) / (M - 1.0)) * (1.0 / (R * C));

        return G0 * (1.0 + s / wz_esr) / (1.0 + s / wp);
    }
}

std::complex<double> BoostConverter::getGv(double freq) {
    double s_val = 2.0 * std::numbers::pi * freq;
    std::complex<double> s(0, s_val);
    
    double D1 = m_params.d1();
    double M = m_params.M();
    double R = std::max(1e-6, m_params.Rload());
    double L = m_params.L();
    double C = m_params.C();
    double rL = m_params.rDcr();
    double rC = m_params.rEsr();
    double rC_safe = std::max(1e-9, rC);
    double wz_esr = 1.0 / (rC_safe * C);

    if (m_params.mode() == OperationMode::CCM) {
        double D_prime = 1.0 - D1;
        double Reff = R * std::pow(D_prime, 2);
        double Gain_dcr = 1.0 / (1.0 + rL / Reff);

        double Gvg0 = (1.0 / D_prime) * Gain_dcr;
        double w0 = D_prime / std::sqrt(L * C);

        std::complex<double> num = Gvg0 * (1.0 + s / wz_esr);
        std::complex<double> den = 1.0 + s * (L/Reff + (rL + rC)*C) + (s * s / (w0 * w0));
        return num / den;

    } else {
        // --- [DCM Gvg: 4700dB 버그 수정 지점] ---
        // [수정] 정확한 소신호 DCM Gvg 게인 수식 적용
        double Gvg0 = M * (M - 1.0) / (2.0 * M - 1.0); 
        double wp = ((2.0 * M - 1.0) / (M - 1.0)) * (1.0 / (R * C));

        // 4700dB가 나왔던 이유는 Gvg0가 M으로 설정되었거나 
        // 미초기화된 rC값이 wz_esr을 통해 게인을 폭발시켰기 때문입니다.
        return Gvg0 * (1.0 + s / wz_esr) / (1.0 + s / wp);
    }
}