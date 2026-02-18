#include "core/ConverterBase.h"

ConverterBase::ConverterBase(const PowerStageParams& params)
    : m_params(params)
{
}

void ConverterBase::setParams(const PowerStageParams& params)
{
    m_params = params;
}

std::complex<double> ConverterBase::getS(double freq) const
{
    const double pi = std::acos(-1.0);
    // s = j * 2 * pi * f
    return std::complex<double>(0, 2.0 * pi * freq);
}