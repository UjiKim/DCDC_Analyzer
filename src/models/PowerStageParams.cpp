#include "models/PowerStageParams.h"

PowerStageParams::PowerStageParams()
    : m_topology(Topology::Buck), m_vin(12.0), m_vout(5.0), m_iout(1.0)
    , m_fsw(1e-6), m_L(10e-6), m_C(10e-6)
    , m_d1(0.0), m_d2(0.0), m_M(0.0), m_K(0.0), m_Rload(0.0)
{
    updateInternalState(); // 초기값 동기화
}

void PowerStageParams::updateInternalState() {
    m_Rload = (m_iout > 1e-6) ? (m_vout / m_iout) : 1e6;
    m_M = m_vout / m_vin;
    m_K = (2.0 * m_L * m_fsw) / m_Rload; // K = 2L / (R * Tsw)

    if (m_topology == Topology::Buck) {
        double D_ideal = m_M; // CCM 기준 Duty
        double Kcrit = 1.0 - D_ideal; //
        
        if (m_K > Kcrit) {
            m_mode = OperationMode::CCM;
            m_d1 = D_ideal;
            m_d2 = 1.0 - m_d1;
        } else {
            m_mode = OperationMode::DCM;
            m_d1 = m_M * std::sqrt(m_K / (1.0 - m_M)); //
            m_d2 = m_d1 * (1.0 - m_M) / m_M; //
        }
    } else { // Boost
        double D_ideal = (m_M - 1.0) / m_M; // CCM 기준 Duty
        double Kcrit = D_ideal * std::pow(1.0 - D_ideal, 2); //
        
        if (m_K > Kcrit) {
            m_mode = OperationMode::CCM;
            m_d1 = D_ideal;
            m_d2 = 1.0 - m_d1;
        } else {
            m_mode = OperationMode::DCM;
            m_d1 = std::sqrt(m_K * m_M * (m_M - 1.0)); //
            m_d2 = m_d1 / (m_M - 1.0); //
        }
    }
}

double PowerStageParams::calculateD1() const {
    if (m_topology == Topology::Buck) {
        if (m_M >= 1.0) return 0.99; // Buck Limit
        double Kcrit = 1.0 - m_M; //
        return (m_K > Kcrit) ? m_M : m_M * std::sqrt(m_K / (1.0 - m_M)); //
    } else { // Boost
        if (m_M <= 1.0) return 0.01; // Boost Limit
        double Kcrit = (m_M - 1.0) / (std::pow(m_M, 3)); //
        return (m_K > Kcrit) ? (m_M - 1.0) / m_M : std::sqrt(m_K * m_M * (m_M - 1.0)); //
    }
}

double PowerStageParams::calculateD2() const {
    if (m_topology == Topology::Buck) {
        // M = D1 / (D1 + D2)
        return m_d1 * (1.0 - m_M) / m_M;
    } else {
        // M = (D1 + D2) / D2
        return m_d1 / (m_M - 1.0);
    }
}