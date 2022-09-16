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
 * RosenbrockMain.cpp
 *
 *  Created on: Mar 23, 2018
 */

#include "Plato_RosenbrockApp.hpp"
#include "Plato_Interface.hpp"

#include "Serializable.hpp"

#include <fstream>
#include <string>
#include <cstdlib>

#ifndef NDEBUG
#include <fenv.h>
#endif

/******************************************************************************/
int main(int aArgc, char **aArgv)
/******************************************************************************/
{

#ifndef NDEBUG
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif

    MPI_Init(&aArgc, &aArgv);

    /************************* CREATE PLATO INTERFACE *************************/
    const bool tLoadFromXML = std::getenv("PLATO_LOAD_FROM_XML") != nullptr;
    Plato::Interface* tPlatoInterface = nullptr;
    try
    {
        if(tLoadFromXML)
        {
            tPlatoInterface = new Plato::Interface(Plato::LoadFromXMLTag{});
            Plato::loadFromXML(*tPlatoInterface, "Interface", "save_state.xml");
            tPlatoInterface->initializePerformerMPI();
            tPlatoInterface->setPerformerOnStages();
            tPlatoInterface->initializeConsole();
        } else {
            tPlatoInterface = new Plato::Interface();
        }
    }
    catch(...)
    {
        MPI_Finalize();
        exit(0);
    }
    /************************* CREATE PLATO INTERFACE *************************/

    /*************************** SET PLATO INTERFACE **************************/
    MPI_Comm tLocalComm;
    tPlatoInterface->getLocalComm(tLocalComm);
    /*************************** SET PLATO INTERFACE **************************/

    /************************ CREATE LOCAL APPLICATION ************************/
    Plato::RosenbrockApp<double>* tMyApp = nullptr;
    try
    {
        tMyApp = new Plato::RosenbrockApp<double>;
    }
    catch(...)
    {
        MPI_Finalize();
        exit(0);
    }
    /************************ CREATE LOCAL APPLICATION ************************/

    /************************** REGISTER APPLICATION **************************/
    try
    {
        tPlatoInterface->registerApplicationNoInitialization(tMyApp);


        Plato::tryFCatchInterfaceExceptions(
            [tMyApp](){tMyApp->initialize();}, 
            *tPlatoInterface);

        if(tLoadFromXML){
            tPlatoInterface->initializeSharedDataMPI();
        } else {
            Plato::tryFCatchInterfaceExceptions(
                [tPlatoInterface, tMyApp](){tPlatoInterface->createSharedData(tMyApp);}, 
                *tPlatoInterface);
            Plato::tryFCatchInterfaceExceptions(
                [tPlatoInterface](){tPlatoInterface->createStages();}, 
                *tPlatoInterface);
        }

    }
    catch(...)
    {
        MPI_Finalize();
        exit(0);
    }
    /************************** REGISTER APPLICATION **************************/

    if(!tLoadFromXML){
        Plato::saveToXML(*tPlatoInterface, "Interface", "save_state_rosenbrock.xml");
    }
    /******************************** PERFORM *********************************/
    try
    {
        tPlatoInterface->perform();
    }
    catch(...)
    {
    }
    /******************************** PERFORM *********************************/

    delete tMyApp;

    MPI_Finalize();

} // RosenbrockMain
