#include "IEncoder.h"
#include "encoders/Encoder.h"

std::shared_ptr<IEncoder> _stdcall CreateEncoder(const EncoderDesc* desc)
{
	return std::make_shared<Encoder>(*desc);
}
