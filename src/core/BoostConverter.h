#ifndef BOOSTCONVERTER_H
#define BOOSTCONVERTER_H

#include "core/ConverterBase.h"

class BoostConverter : public ConverterBase {
public:
    using ConverterBase::ConverterBase; // 생성자 상속

    // 제어-출력 전달 함수 Gvd(s)
    std::complex<double> getGd(double freq) override;

    // 오디오 감쇄능(입력-출력) 전달 함수 Gvg(s)
    std::complex<double> getGv(double freq) override;
};

#endif