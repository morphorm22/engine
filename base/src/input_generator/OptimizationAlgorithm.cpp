#include "OptimizationAlgorithm.hpp"
#include "pugixml.hpp"
#include "XMLGeneratorUtilities.hpp"


OptimizationAlgorithm::OptimizationAlgorithm(const OptimizationParameters& aParameters)
{

}

//****************************PLATO **********************************************//
OptimizationAlgorithmPlatoOC::OptimizationAlgorithmPlatoOC(const OptimizationParameters& aParameters)
{

}
void OptimizationAlgorithmPlatoOC::writeInterface(pugi::xml_node& aNode)
{

}

void OptimizationAlgorithmPlatoOC::writeAuxiliaryFiles(pugi::xml_node& aNode)
{

}

OptimizationAlgorithmPlatoKSBC::OptimizationAlgorithmPlatoKSBC(const OptimizationParameters& aParameters)
{

}
void OptimizationAlgorithmPlatoKSBC::writeInterface(pugi::xml_node& aNode)
{

}

void OptimizationAlgorithmPlatoKSBC::writeAuxiliaryFiles(pugi::xml_node& aNode)
{

}

//****************************ROL **********************************************//
OptimizationAlgorithmROL::OptimizationAlgorithmROL(const OptimizationParameters& aParameters)
{

}
void OptimizationAlgorithmROL::writeInterface(pugi::xml_node& aNode)
{

}

void OptimizationAlgorithmROL::writeAuxiliaryFiles(pugi::xml_node& aNode)
{

}

//****************************Dakota **********************************************//
OptimizationAlgorithmDakota::OptimizationAlgorithmDakota(const OptimizationParameters& aParameters)
{

}
void OptimizationAlgorithmDakota::writeInterface(pugi::xml_node& aNode)
{

}

void OptimizationAlgorithmDakota::writeAuxiliaryFiles(pugi::xml_node& aNode)
{

}