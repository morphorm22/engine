/*
//@HEADER
// *************************************************************************
//   Plato Engine v.1.0: Copyright 2018, National Technology & Engineering
//                    Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Sandia Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact the Plato team (plato3D-help@sandia.gov)
//
// *************************************************************************
//@HEADER
*/

/*
 * Plato_Main.cpp
 *
 *  Created on: Nov 21, 2016
 *
 */

#include "communicator.hpp"

#ifdef GEOMETRY
#include <Kokkos_Core.hpp>
#endif

#include <iostream>
#include <sstream>

#include "PlatoApp.hpp"
#include "Plato_Exceptions.hpp"
#include "Plato_Interface.hpp"
#include "Plato_DriverFactory.hpp"

#include <iostream>
#include <sstream>

#ifndef NDEBUG
#include <fenv.h>
#endif

#include "Plato_SerializationHeaders.hpp"
#include "Plato_SerializationUtilities.hpp"

#include <fstream>
#include <string>

#include <cstdlib>

const auto kInterfaceXMLFileName = Plato::XMLFileName{"save_main_interface.xml"};
const auto kInterfaceXMLNodeName = Plato::XMLNodeName{"Interface"};
const auto kAppXMLFileName = Plato::XMLFileName{"save_main_app.xml"};
const auto kAppXMLNodeName = Plato::XMLNodeName{"App"};

void writeSplashScreen();

/******************************************************************************/
int main(int aArgc, char *aArgv[])
/******************************************************************************/
{
#ifndef NDEBUG
    feclearexcept(FE_ALL_EXCEPT);
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif

    MPI_Init(&aArgc, (char***) &aArgv);

#if defined(GEOMETRY) || defined(AMFILTER_ENABLED)
    Kokkos::initialize(aArgc, aArgv);
#endif

    Plato::Interface* tPlatoInterface = nullptr;
    PlatoApp *tPlatoApp = nullptr;

    auto safeExit = [&]() {
        delete tPlatoInterface;
        delete tPlatoApp;
#if defined(GEOMETRY) || defined(AMFILTER_ENABLED)
        Kokkos::finalize();
#endif
        MPI_Finalize();
        exit(0);
    };

    const bool tLoadFromXML = std::getenv("PLATO_LOAD_FROM_XML") != nullptr;
    try
    {
        if(tLoadFromXML) 
        {
            tPlatoInterface = new Plato::Interface(kInterfaceXMLFileName, kInterfaceXMLNodeName);
        } 
        else 
        {
            tPlatoInterface = new Plato::Interface();
        }
    }
    catch(Plato::ParsingException& e)
    {
        std::cout << e.message() << std::endl;
        safeExit();
    }
    catch(...)
    {
        safeExit();
    }

    MPI_Comm tLocalComm;
    tPlatoInterface->getLocalComm(tLocalComm);

    // Create Plato services application and register it with the Plato interface
    try
    {
        if(aArgc > static_cast<int>(1))
        {
            tPlatoApp = new PlatoApp(aArgc, aArgv, tLocalComm);
        }
        else
        {
            tPlatoApp = new PlatoApp(tLocalComm);
        }
    }
    catch(...)
    {
        tPlatoApp = nullptr;
        tPlatoInterface->Catch();
    }

    try
    {
        if(tLoadFromXML)
        {
            Plato::registerApplicationWithInterfaceLoadFromXML(
                *tPlatoInterface, tPlatoApp, kAppXMLFileName, kAppXMLNodeName);
        } 
        else 
        {
            tPlatoInterface->registerApplication(tPlatoApp);
        }
    }
    catch(...)
    {
        safeExit();
    }

    writeSplashScreen();

    const bool tSaveToXML = std::getenv("PLATO_SAVE_TO_XML") != nullptr;
    if(tSaveToXML){
        Plato::saveToXML(*tPlatoInterface, kInterfaceXMLFileName, kInterfaceXMLNodeName);
        Plato::saveToXML(*tPlatoApp, kAppXMLFileName, kAppXMLNodeName);
    } 
    try
    {
        // Note: This code is encapsulated and is part of
        // Interface::run() in Plato_Interface. However doing so pulls
        // all of the drivers into the interface library. It also
        // requires the driver library be included when building
        // Analyze_MPMD. Future works is to refactor.

        // This handleException matches the one in Interface::perform().
        tPlatoInterface->handleExceptions();

        // There should be at least one driver block but there can
        // be more. These additional driver blocks can be in serial
        // or nested. The while loop coupled with factory processes
        // driver blocks that are serial. Nested driver blocks
        // are processed recursively via the EngineObjective.
        Plato::DriverFactory<double> tDriverFactory;
        Plato::DriverInterface<double>* tDriver = nullptr;

        // Note: When first called, the factory will look for the
        // first driver block. Subsequent calls will look for the
        // next driver block if it exists.
        while((tDriver =
               tDriverFactory.create(tPlatoInterface, tLocalComm)) != nullptr)
        {
            tDriver->run();

            // If the last driver compute the final stage before
            // deleting it. The driver finalize calls the interface
            // finalize with possibly a stage to compute.
            if( tDriver->lastDriver() )
            {
                tDriver->finalize();
            }

            delete tDriver;
        }
    }
    catch(...)
    {
        safeExit();
    }

    safeExit();
}


void writeSplashScreen()
{
  std::stringstream tSplash;
  tSplash << "####################################################################################" << std::endl;
  tSplash << "#                                                                                  #" << std::endl;
  tSplash << "#                                                                                  #" << std::endl;
  tSplash << "#  888b     d888                           888                                     #" << std::endl;
  tSplash << "#  8888b   d8888                           888                                     #" << std::endl;
  tSplash << "#  88888b.d88888                           888                                     #" << std::endl;
  tSplash << "#  888Y88888P888  .d88b.  888d888 88888b.  88888b.   .d88b.  888d888 88888b.d88b.  #" << std::endl;
  tSplash << "#  888 Y888P 888 d88''88b 888P'   888 '88b 888 '88b d88''88b 888P'   888 '888 '88b #" << std::endl;
  tSplash << "#  888  Y8P  888 888  888 888     888  888 888  888 888  888 888     888  888  888 #" << std::endl;
  tSplash << "#  888   '   888 Y88..88P 888     888 d88P 888  888 Y88..88P 888     888  888  888 #" << std::endl;
  tSplash << "#  888       888  'Y88P'  888     88888P'  888  888  'Y88P'  888     888  888  888 #" << std::endl;
  tSplash << "#                                 888                                              #" << std::endl;
  tSplash << "#                                 888  DIGITAL ENGINEERING INNOVATORS              #" << std::endl;
  tSplash << "#                                 888                                              #" << std::endl;
  tSplash << "#                                      Questions? contact info@morphorm.com        #" << std::endl;
  tSplash << "#                                                                                  #" << std::endl;
  tSplash << "####################################################################################" << std::endl;
  Plato::Console::Alert(tSplash.str());
}
