// PlatoSubproblemLibraryVersion(8): a stand-alone library for the kernel filter for plato.
#include "PSL_KernelThenHeavisideFilter.hpp"
#include "PSL_FreeHelpers.hpp"
#include "PSL_Interface_ParallelVector.hpp"

namespace PlatoSubproblemLibrary
{

void KernelThenHeavisideFilter::projection_apply(const double& beta, AbstractInterface::ParallelVector* field) const
{
    auto tFieldLength = field->get_length();
    for(size_t i = 0; i < tFieldLength; ++i)
    {
        field->set_value(i,heaviside_apply(beta,field->get_value(i)));
    }
}
void KernelThenHeavisideFilter::projection_gradient(const double& beta, AbstractInterface::ParallelVector* const field, AbstractInterface::ParallelVector* gradient) const
{
    size_t tLength = gradient->get_length();
    for(size_t i = 0u; i < tLength; ++i)
    {
        gradient->set_value(i, gradient->get_value(i)*heaviside_gradient(beta,field->get_value(i)));
    }
}

}
