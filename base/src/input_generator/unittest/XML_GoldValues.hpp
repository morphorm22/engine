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
 * XML_GoldValues.hpp
 *
 *  Created on: Sep 26, 2019
 */


namespace PlatoTestXMLGenerator
{
std::string gMechanicalGoldString =
        "<?xml version=\"1.0\"?>\n"
        "<ParameterList name=\"Problem\">\n"
        "<Parameter name=\"Physics\" type=\"string\" value=\"Plato Driver\" />\n"
        "<Parameter name=\"Spatial Dimension\" type=\"int\" value=\"3\" />\n"
        "<Parameter name=\"Input Mesh\" type=\"string\" value=\"\" />\n"
        "<ParameterList name=\"Plato Problem\">\n"
        "<Parameter name=\"Physics\" type=\"string\" value=\"Mechanical\" />\n"
        "<Parameter name=\"PDE Constraint\" type=\"string\" value=\"Elliptic\" />\n"
        "<Parameter name=\"Constraint\" type=\"string\" value=\"My Volume\" />\n"
        "<Parameter name=\"Objective\" type=\"string\" value=\"My Internal Elastic Energy\" />\n"
        "<Parameter name=\"Self-Adjoint\" type=\"bool\" value=\"true\" />\n"
        "<ParameterList name=\"My Volume\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Scalar Function\" />\n"
        "<Parameter name=\"Scalar Function Type\" type=\"string\" value=\"Volume\" />\n"
        "<ParameterList name=\"Penalty Function\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"SIMP\" />\n"
        "<Parameter name=\"Exponent\" type=\"double\" value=\"1.0\" />\n"
        "<Parameter name=\"Minimum Value\" type=\"double\" value=\"0.0\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"My Internal Elastic Energy\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Scalar Function\" />\n"
        "<ParameterList name=\"Penalty Function\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"SIMP\" />\n"
        "<Parameter name=\"Exponent\" type=\"double\" value=\"3.0\" />\n"
        "<Parameter name=\"Minimum Value\" type=\"double\" value=\"1.0e-3\" />\n"
        "</ParameterList>\n"
        "<Parameter name=\"Scalar Function Type\" type=\"string\" value=\"Internal Elastic Energy\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Elliptic\">\n"
        "<ParameterList name=\"Penalty Function\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"SIMP\" />\n"
        "<Parameter name=\"Exponent\" type=\"double\" value=\"3.0\" />\n"
        "<Parameter name=\"Minimum Value\" type=\"double\" value=\"1e-3\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Material Model\">\n"
        "<ParameterList name=\"Isotropic Linear Elastic\">\n"
        "<Parameter name=\"Poissons Ratio\" type=\"double\" value=\"0.3\" />\n"
        "<Parameter name=\"Youngs Modulus\" type=\"double\" value=\"1e8\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Natural Boundary Conditions\">\n"
        "<ParameterList name=\"Traction Vector Boundary Condition 1\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Uniform\" />\n"
        "<Parameter name=\"Values\" type=\"Array(double)\" value=\"{0.000000,-3000.000000,0.000000}\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"2\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Essential Boundary Conditions\">\n"
        "<ParameterList name=\"X Fixed Displacement Boundary Condition 1\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"0\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"1\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Y Fixed Displacement Boundary Condition 2\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"1\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"1\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Z Fixed Displacement Boundary Condition 3\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"2\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"1\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"X Fixed Displacement Boundary Condition 4\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"0\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"1\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Y Fixed Displacement Boundary Condition 5\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"1\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"1\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Fixed Value\" />\n"
        "<Parameter name=\"Value\" type=\"double\" value=\"3.0\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
;

std::string gThermalGoldString =
        "<?xml version=\"1.0\"?>\n"
        "<ParameterList name=\"Problem\">\n"
        "<Parameter name=\"Physics\" type=\"string\" value=\"Plato Driver\" />\n"
        "<Parameter name=\"Spatial Dimension\" type=\"int\" value=\"3\" />\n"
        "<Parameter name=\"Input Mesh\" type=\"string\" value=\"\" />\n"
        "<ParameterList name=\"Plato Problem\">\n"
        "<Parameter name=\"Physics\" type=\"string\" value=\"Thermal\" />\n"
        "<Parameter name=\"PDE Constraint\" type=\"string\" value=\"Elliptic\" />\n"
        "<Parameter name=\"Constraint\" type=\"string\" value=\"My Volume\" />\n"
        "<Parameter name=\"Objective\" type=\"string\" value=\"My Internal Thermal Energy\" />\n"
        "<Parameter name=\"Self-Adjoint\" type=\"bool\" value=\"false\" />\n"
        "<ParameterList name=\"My Volume\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Scalar Function\" />\n"
        "<Parameter name=\"Scalar Function Type\" type=\"string\" value=\"Volume\" />\n"
        "<ParameterList name=\"Penalty Function\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"SIMP\" />\n"
        "<Parameter name=\"Exponent\" type=\"double\" value=\"1.0\" />\n"
        "<Parameter name=\"Minimum Value\" type=\"double\" value=\"0.0\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"My Internal Thermal Energy\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Scalar Function\" />\n"
        "<ParameterList name=\"Penalty Function\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"SIMP\" />\n"
        "<Parameter name=\"Exponent\" type=\"double\" value=\"3.0\" />\n"
        "</ParameterList>\n"
        "<Parameter name=\"Scalar Function Type\" type=\"string\" value=\"Internal Thermal Energy\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Elliptic\">\n"
        "<ParameterList name=\"Penalty Function\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"SIMP\" />\n"
        "<Parameter name=\"Exponent\" type=\"double\" value=\"3.0\" />\n"
        "<Parameter name=\"Minimum Value\" type=\"double\" value=\"1e-3\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Material Model\">\n"
        "<ParameterList name=\"Isotropic Linear Thermal\">\n"
        "<Parameter name=\"Conductivity Coefficient\" type=\"double\" value=\"210.0\" />\n"
        "<Parameter name=\"Mass Density\" type=\"double\" value=\"2703\" />\n"
        "<Parameter name=\"Specific Heat\" type=\"double\" value=\"900\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Natural Boundary Conditions\">\n"
        "<ParameterList name=\"Surface Flux Boundary Condition 1\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Uniform\" />\n"
        "<Parameter name=\"Value\" type=\"double\" value=\"-1e2\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"ss_1\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Essential Boundary Conditions\">\n"
        "<ParameterList name=\"Fixed Temperature Boundary Condition 1\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"0\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"1\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Fixed Temperature Boundary Condition 2\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"0\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"2\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Fixed Temperature Boundary Condition 3\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"0\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"3\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Fixed Value\" />\n"
        "<Parameter name=\"Value\" type=\"double\" value=\"25.0\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "</ParameterList>\n";

std::string gThermomechanicalGoldString =
        "<?xml version=\"1.0\"?>\n"
        "<ParameterList name=\"Problem\">\n"
        "<Parameter name=\"Physics\" type=\"string\" value=\"Plato Driver\" />\n"
        "<Parameter name=\"Spatial Dimension\" type=\"int\" value=\"3\" />\n"
        "<Parameter name=\"Input Mesh\" type=\"string\" value=\"\" />\n"
        "<ParameterList name=\"Plato Problem\">\n"
        "<Parameter name=\"Physics\" type=\"string\" value=\"Thermomechanical\" />\n"
        "<Parameter name=\"PDE Constraint\" type=\"string\" value=\"Elliptic\" />\n"
        "<Parameter name=\"Constraint\" type=\"string\" value=\"My Volume\" />\n"
        "<Parameter name=\"Objective\" type=\"string\" value=\"My Internal Thermoelastic Energy\" />\n"
        "<Parameter name=\"Self-Adjoint\" type=\"bool\" value=\"false\" />\n"
        "<ParameterList name=\"My Volume\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Scalar Function\" />\n"
        "<Parameter name=\"Scalar Function Type\" type=\"string\" value=\"Volume\" />\n"
        "<ParameterList name=\"Penalty Function\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"SIMP\" />\n"
        "<Parameter name=\"Exponent\" type=\"double\" value=\"1.0\" />\n"
        "<Parameter name=\"Minimum Value\" type=\"double\" value=\"0.0\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"My Internal Thermoelastic Energy\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Scalar Function\" />\n"
        "<ParameterList name=\"Penalty Function\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"SIMP\" />\n"
        "<Parameter name=\"Exponent\" type=\"double\" value=\"3.0\" />\n"
        "</ParameterList>\n"
        "<Parameter name=\"Scalar Function Type\" type=\"string\" value=\"Internal Thermoelastic Energy\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Elliptic\">\n"
        "<ParameterList name=\"Penalty Function\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"SIMP\" />\n"
        "<Parameter name=\"Exponent\" type=\"double\" value=\"3.0\" />\n"
        "<Parameter name=\"Minimum Value\" type=\"double\" value=\"1e-3\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Newton Iteration\">\n"
        "<Parameter name=\"Maximum Iterations\" type=\"int\" value=\"2\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Material Model\">\n"
        "<ParameterList name=\"Isotropic Linear Thermoelastic\">\n"
        "<Parameter name=\"Poissons Ratio\" type=\"double\" value=\"0.3\" />\n"
        "<Parameter name=\"Youngs Modulus\" type=\"double\" value=\"1e11\" />\n"
        "<Parameter name=\"Thermal Expansivity\" type=\"double\" value=\"1e-5\" />\n"
        "<Parameter name=\"Thermal Conductivity\" type=\"double\" value=\"910.0\" />\n"
        "<Parameter name=\"Reference Temperature\" type=\"double\" value=\"1e-2\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Mechanical Natural Boundary Conditions\">\n"
        "<ParameterList name=\"Traction Vector Boundary Condition 1\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Uniform\" />\n"
        "<Parameter name=\"Values\" type=\"Array(double)\" value=\"{0.000000,100000.000000,0.000000}\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"ss_1\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Thermal Natural Boundary Conditions\">\n"
        "<ParameterList name=\"Surface Flux Boundary Condition 2\">\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Uniform\" />\n"
        "<Parameter name=\"Value\" type=\"double\" value=\"0.0\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"ss_1\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Essential Boundary Conditions\">\n"
        "<ParameterList name=\"Y Fixed Displacement Boundary Condition 1\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"1\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"1\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Z Fixed Displacement Boundary Condition 2\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"2\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"1\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"X Fixed Displacement Boundary Condition 3\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"0\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"11\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Y Fixed Displacement Boundary Condition 4\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"1\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"2\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Z Fixed Displacement Boundary Condition 5\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"2\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"2\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"X Fixed Displacement Boundary Condition 6\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"0\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"21\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Fixed Temperature Boundary Condition 7\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"3\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"1\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "<ParameterList name=\"Fixed Temperature Boundary Condition 8\">\n"
        "<Parameter name=\"Index\" type=\"int\" value=\"3\" />\n"
        "<Parameter name=\"Sides\" type=\"string\" value=\"2\" />\n"
        "<Parameter name=\"Type\" type=\"string\" value=\"Zero Value\" />\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "</ParameterList>\n"
        "</ParameterList>\n";

std::string gInterfaceXMLCompMinTOPAGoldString =


"<?xml version=\"1.0\"?>\n"
"<Console>\n"
  "<Verbose>true</Verbose>\n"
  "<Enabled>true</Enabled>\n"
"</Console>\n"
"<Performer>\n"
  "<Name>PlatoMain</Name>\n"
  "<Code>PlatoMain</Code>\n"
  "<PerformerID>0</PerformerID>\n"
"</Performer>\n"
"<Performer>\n"
  "<Name>plato_analyze_1</Name>\n"
  "<Code>plato_analyze</Code>\n"
  "<PerformerID>1</PerformerID>\n"
"</Performer>\n"
"<SharedData>\n"
  "<Name>Objective Value 1</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Global</Layout>\n"
  "<Size>1</Size>\n"
  "<OwnerName>plato_analyze_1</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Objective 1 Gradient</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Nodal Field</Layout>\n"
  "<OwnerName>plato_analyze_1</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Objective Value</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Global</Layout>\n"
  "<Size>1</Size>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Objective Gradient</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Nodal Field</Layout>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Optimization DOFs</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Nodal Field</Layout>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
  "<UserName>plato_analyze_1</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Topology</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Nodal Field</Layout>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
  "<UserName>plato_analyze_1</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Volume</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Global</Layout>\n"
  "<Size>1</Size>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Volume Gradient</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Nodal Field</Layout>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Design Volume</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Global</Layout>\n"
  "<Size>1</Size>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Lower Bound Value</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Global</Layout>\n"
  "<Size>1</Size>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Upper Bound Value</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Global</Layout>\n"
  "<Size>1</Size>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Lower Bound Vector</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Nodal Field</Layout>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<SharedData>\n"
  "<Name>Upper Bound Vector</Name>\n"
  "<Type>Scalar</Type>\n"
  "<Layout>Nodal Field</Layout>\n"
  "<OwnerName>PlatoMain</OwnerName>\n"
  "<UserName>PlatoMain</UserName>\n"
"</SharedData>\n"
"<Stage>\n"
  "<Name>Output To File</Name>\n"
  "<Operation>\n"
    "<Name>PlatoMainOutput</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Topology</ArgumentName>\n"
      "<SharedDataName>Topology</SharedDataName>\n"
    "</Input>\n"
    "<Input>\n"
      "<ArgumentName>Objective Gradient</ArgumentName>\n"
      "<SharedDataName>Objective Gradient</SharedDataName>\n"
    "</Input>\n"
    "<Input>\n"
      "<ArgumentName>Optimization DOFs</ArgumentName>\n"
      "<SharedDataName>Optimization DOFs</SharedDataName>\n"
    "</Input>\n"
  "</Operation>\n"
"</Stage>\n"
"<Stage>\n"
  "<Name>Update Problem</Name>\n"
  "<Operation>\n"
    "<Name>Update Problem</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
  "</Operation>\n"
"</Stage>\n"
"<Stage>\n"
  "<Name>Cache State</Name>\n"
"</Stage>\n"
"<Stage>\n"
  "<Name>Set Lower Bounds</Name>\n"
  "<Output>\n"
    "<SharedDataName>Lower Bound Vector</SharedDataName>\n"
  "</Output>\n"
"</Stage>\n"
"<Stage>\n"
  "<Name>Set Upper Bounds</Name>\n"
  "<Output>\n"
    "<SharedDataName>Upper Bound Vector</SharedDataName>\n"
  "</Output>\n"
"</Stage>\n"
"<Stage>\n"
  "<Name>Design Volume</Name>\n"
  "<Operation>\n"
    "<Name>Design Volume</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Output>\n"
      "<ArgumentName>Design Volume</ArgumentName>\n"
      "<SharedDataName>Design Volume</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Output>\n"
    "<SharedDataName>Design Volume</SharedDataName>\n"
  "</Output>\n"
"</Stage>\n"
"<Stage>\n"
  "<Name>Volume</Name>\n"
  "<Input>\n"
    "<SharedDataName>Optimization DOFs</SharedDataName>\n"
  "</Input>\n"
  "<Operation>\n"
    "<Name>FilterControl</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Field</ArgumentName>\n"
      "<SharedDataName>Optimization DOFs</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Filtered Field</ArgumentName>\n"
      "<SharedDataName>Topology</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Operation>\n"
    "<Name>Compute Current Volume</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Topology</ArgumentName>\n"
      "<SharedDataName>Topology</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Volume</ArgumentName>\n"
      "<SharedDataName>Volume</SharedDataName>\n"
    "</Output>\n"
    "<Output>\n"
      "<ArgumentName>Volume Gradient</ArgumentName>\n"
      "<SharedDataName>Volume Gradient</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Output>\n"
    "<SharedDataName>Volume</SharedDataName>\n"
  "</Output>\n"
"</Stage>\n"
"<Stage>\n"
  "<Name>Volume Gradient</Name>\n"
  "<Input>\n"
    "<SharedDataName>Optimization DOFs</SharedDataName>\n"
  "</Input>\n"
  "<Operation>\n"
    "<Name>FilterControl</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Field</ArgumentName>\n"
      "<SharedDataName>Optimization DOFs</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Filtered Field</ArgumentName>\n"
      "<SharedDataName>Topology</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Operation>\n"
    "<Name>Compute Current Volume</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Topology</ArgumentName>\n"
      "<SharedDataName>Topology</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Volume</ArgumentName>\n"
      "<SharedDataName>Volume</SharedDataName>\n"
    "</Output>\n"
    "<Output>\n"
      "<ArgumentName>Volume Gradient</ArgumentName>\n"
      "<SharedDataName>Volume Gradient</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Operation>\n"
    "<Name>FilterGradient</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Field</ArgumentName>\n"
      "<SharedDataName>Optimization DOFs</SharedDataName>\n"
    "</Input>\n"
    "<Input>\n"
      "<ArgumentName>Gradient</ArgumentName>\n"
      "<SharedDataName>Volume Gradient</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Filtered Gradient</ArgumentName>\n"
      "<SharedDataName>Volume Gradient</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Output>\n"
    "<SharedDataName>Volume Gradient</SharedDataName>\n"
  "</Output>\n"
"</Stage>\n"
"<Stage>\n"
  "<Name>Objective Value</Name>\n"
  "<Input>\n"
    "<SharedDataName>Optimization DOFs</SharedDataName>\n"
  "</Input>\n"
  "<Operation>\n"
    "<Name>FilterControl</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Field</ArgumentName>\n"
      "<SharedDataName>Optimization DOFs</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Filtered Field</ArgumentName>\n"
      "<SharedDataName>Topology</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Operation>\n"
    "<Name>Compute Objective Value</Name>\n"
    "<PerformerName>plato_analyze_1</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Topology</ArgumentName>\n"
      "<SharedDataName>Topology</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Objective Value</ArgumentName>\n"
      "<SharedDataName>Objective Value 1</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Operation>\n"
    "<Name>AggregateEnergy</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Value 1</ArgumentName>\n"
      "<SharedDataName>Objective Value 1</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Value</ArgumentName>\n"
      "<SharedDataName>Objective Value</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Output>\n"
    "<SharedDataName>Objective Value</SharedDataName>\n"
  "</Output>\n"
"</Stage>\n"
"<Stage>\n"
  "<Name>Objective Gradient</Name>\n"
  "<Input>\n"
    "<SharedDataName>Optimization DOFs</SharedDataName>\n"
  "</Input>\n"
  "<Operation>\n"
    "<Name>FilterControl</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Field</ArgumentName>\n"
      "<SharedDataName>Optimization DOFs</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Filtered Field</ArgumentName>\n"
      "<SharedDataName>Topology</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Operation>\n"
    "<Name>Compute Objective Gradient</Name>\n"
    "<PerformerName>plato_analyze_1</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Topology</ArgumentName>\n"
      "<SharedDataName>Topology</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Objective Gradient</ArgumentName>\n"
      "<SharedDataName>Objective 1 Gradient</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Operation>\n"
    "<Name>AggregateGradient</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Field 1</ArgumentName>\n"
      "<SharedDataName>Objective 1 Gradient</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Field</ArgumentName>\n"
      "<SharedDataName>Objective Gradient</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Operation>\n"
    "<Name>FilterGradient</Name>\n"
    "<PerformerName>PlatoMain</PerformerName>\n"
    "<Input>\n"
      "<ArgumentName>Field</ArgumentName>\n"
      "<SharedDataName>Optimization DOFs</SharedDataName>\n"
    "</Input>\n"
    "<Input>\n"
      "<ArgumentName>Gradient</ArgumentName>\n"
      "<SharedDataName>Objective Gradient</SharedDataName>\n"
    "</Input>\n"
    "<Output>\n"
      "<ArgumentName>Filtered Gradient</ArgumentName>\n"
      "<SharedDataName>Objective Gradient</SharedDataName>\n"
    "</Output>\n"
  "</Operation>\n"
  "<Output>\n"
    "<SharedDataName>Objective Gradient</SharedDataName>\n"
  "</Output>\n"
"</Stage>\n"
"<Optimizer>\n"
  "<Options />\n"
  "<Output>\n"
    "<OutputStage>Output To File</OutputStage>\n"
  "</Output>\n"
  "<CacheStage>\n"
    "<Name>Cache State</Name>\n"
  "</CacheStage>\n"
  "<UpdateProblemStage>\n"
    "<Name>Update Problem</Name>\n"
  "</UpdateProblemStage>\n"
  "<OptimizationVariables>\n"
    "<ValueName>Optimization DOFs</ValueName>\n"
    "<InitializationStage>Initialize Optimization</InitializationStage>\n"
    "<FilteredName>Topology</FilteredName>\n"
    "<LowerBoundValueName>Lower Bound Value</LowerBoundValueName>\n"
    "<LowerBoundVectorName>Lower Bound Vector</LowerBoundVectorName>\n"
    "<UpperBoundValueName>Upper Bound Value</UpperBoundValueName>\n"
    "<UpperBoundVectorName>Upper Bound Vector</UpperBoundVectorName>\n"
    "<SetLowerBoundsStage>Set Lower Bounds</SetLowerBoundsStage>\n"
    "<SetUpperBoundsStage>Set Upper Bounds</SetUpperBoundsStage>\n"
  "</OptimizationVariables>\n"
  "<Objective>\n"
    "<ValueName>Objective Value</ValueName>\n"
    "<ValueStageName>Objective Value</ValueStageName>\n"
    "<GradientName>Objective Gradient</GradientName>\n"
    "<GradientStageName>Objective Gradient</GradientStageName>\n"
  "</Objective>\n"
  "<BoundConstraint>\n"
    "<Upper>1.0</Upper>\n"
    "<Lower>0.0</Lower>\n"
  "</BoundConstraint>\n"
  "<Convergence>\n"
    "<MaxIterations></MaxIterations>\n"
  "</Convergence>\n"
"</Optimizer>\n"
"<mesh>\n"
  "<type>unstructured</type>\n"
  "<format>exodus</format>\n"
  "<mesh></mesh>\n"
"</mesh>\n"
"<output>\n"
  "<file>plato</file>\n"
  "<format>exodus</format>\n"
"</output>\n"
;



} // end PlatoTestXMLGenerator namespace
