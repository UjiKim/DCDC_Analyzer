#ifndef POWERSTAGEPARAMS_H
#define POWERSTAGEPARAMS_H

#include <cmath>
#include <algorithm>

enum class Topology { Buck, Boost };
enum class OperationMode { CCM, DCM };

class PowerStageParams {
public:
    PowerStageParams();

    // --- [Getters] ---
    // 계산된 결과를 즉시 반환 (re-calculation 제거)
    double vin() const { return m_vin; }
    double vout() const { return m_vout; }
    double L() const { return m_L; }
    double C() const { return m_C; }
    double fsw() const { return m_fsw; }
    double rDcr() const { return m_rDcr; }
    double rEsr() const { return m_rEsr; }
    double rOn() const { return m_rOn; }
    double d1() const { return m_d1; }
    double d2() const { return m_d2; }
    double M() const  { return m_M; }  // Conversion Ratio
    double K() const  { return m_K; }  // Dimensionless parameter K
    double Rload() const { return m_Rload; }

    // 기본 파라미터 Getters
    Topology topology() const { return m_topology; }
    OperationMode mode() const { return m_mode; }

    // --- [Setters] ---
    void setTopology(Topology t) { m_topology = t; }
    void setVin(double v) { m_vin = v; }
    void setVout(double v) { m_vout = v; }
    void setIout(double i) { m_iout = i; }
    void setFsw(double f) { m_fsw = f; }
    void setL(double l) { m_L = l; }
    void setC(double c) { m_C = c; }
    void setRdcr(double r) { m_rDcr = r; }
    void setResr(double r) { m_rEsr = r; }
    void setRon(double r) { m_rOn = r; }

    // --- [State Management] ---
    // 모든 파라미터 설정 후 호출하여 내부 멤버 변수들을 일괄 업데이트
    void updateInternalState();

private:
    // 내부 계산 로직 (updateInternalState에서만 호출)
    double calculateD1() const;
    double calculateD2() const;

    // 설계 파라미터 (Input)
    Topology m_topology;
    OperationMode m_mode;
    
    double m_vin, m_vout, m_iout, m_fsw, m_L, m_C;
    double m_rEsr, m_rDcr, m_rOn;

    // 계산된 파라미터 (Cached State)
    double m_d1, m_d2;
    double m_M;      // Vout/Vin
    double m_K;      // 2Lfs/R
    double m_Rload;  // Vout/Iout
};

#endif