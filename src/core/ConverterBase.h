#ifndef CONVERTERBASE_H
#define CONVERTERBASE_H

#include <complex>
#include "models/PowerStageParams.h"

class ConverterBase {
public:
    explicit ConverterBase(const PowerStageParams& params);
    virtual ~ConverterBase() = default;

    // 파라미터 업데이트
    void setParams(const PowerStageParams& params);
    
    // --- [추상 인터페이스: 각 토폴로지에서 구현] ---
    // 1. Control-to-Output Transfer Function: Gd(s)
    virtual std::complex<double> getGd(double freq) = 0;

    // 2. Audio Susceptibility (Input-to-Output): Gv(s)
    virtual std::complex<double> getGv(double freq) = 0;

protected:
    PowerStageParams m_params;
    
    // 복소 주파수 s = j*w 계산 편의 함수
    std::complex<double> getS(double freq) const;
};

#endif // CONVERTERBASE_H