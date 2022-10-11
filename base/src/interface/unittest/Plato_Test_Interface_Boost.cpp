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

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <boost/archive/tmpdir.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "Plato_Operation.hpp"
#include "Plato_SingleOperation.hpp"
#include "Plato_Stage.hpp"

#include "Plato_SerializationHeaders.hpp"

using namespace Plato;


class O
{
public:
  O(){Name = "Base";}

    template<class Archive>
    void serialize(Archive & ar, unsigned int )
    {
       ar & boost::serialization::make_nvp("Name",Name);
    }


void saveXML(std::string aFilename)                                     
    {                                                                   
        std::ofstream tOutFileStream(aFilename.c_str());                    
        boost::archive::xml_oarchive tOutputArchive(tOutFileStream);        
        tOutputArchive << boost::serialization::make_nvp("base",*this);                   
        tOutFileStream.close();                                             
    }   



std::string Name;

};
//BOOST_SERIALIZATION_ASSUME_ABSTRACT( O );

class S : public  O
{
  public:
  S(){Name = "Single";}
  
    template<class Archive>
    void serialize(Archive & ar, unsigned int )
    {
        //ar & boost::serialization::base_object<O>(*this);
        //ar &  BOOST_SERIALIZATION_BASE_OBJECT_NVP(O);
        ar & boost::serialization::make_nvp("Op",boost::serialization::base_object<O>(*this));
        
    
    }

};
//BOOST_CLASS_EXPORT_IMPLEMENT(S)

class M : public  O
{
  public:
  M(){Name = "Multiple";}
  template<class Archive>
    void serialize(Archive & ar, unsigned int )
    {
        //ar & boost::serialization::base_object<O>(*this);
        //ar &  BOOST_SERIALIZATION_BASE_OBJECT_NVP(O);
        ar & boost::serialization::make_nvp("Op",boost::serialization::base_object<O>(*this));
    }

};

class V
{
  public:
  V(){}
   template<class Archive>
    void serialize(Archive & ar, unsigned int )
    {
        ar & boost::serialization::make_nvp("Ops",ops);
    }

  std::vector<O*> ops;

void saveXML(std::string aFilename)                                     
    {                                                                   
        std::ofstream tOutFileStream(aFilename.c_str());                    
        boost::archive::xml_oarchive tOutputArchive(tOutFileStream);        
        tOutputArchive << boost::serialization::make_nvp("vec",*this);        
        //tOutputArchive.flush();           
        //tOutFileStream.close();                                             
    }  


void loadXML(const std::string& aFilename, boost::archive::xml_iarchive& aArchive)
{
  aArchive >> boost::serialization::make_nvp("vec", *this);
}
};

//BOOST_CLASS_EXPORT_KEY(M)
//BOOST_CLASS_EXPORT_IMPLEMENT(M)
//BOOST_CLASS_EXPORT_KEY(S)




TEST(PlatoInterface, StageBoost)
{
  //std::vector<O*> ops;
  //ops.push_back(new S());
  //ops.push_back(new M());

  V stage;
  stage.ops.push_back(new S());
  stage.ops.push_back(new S());
  stage.ops.push_back(new M());
  
  
  std::string aFilename = "tmp.xml";
  stage.saveXML("tmp.xml");
        
  V stage2;

  std::ifstream tInFileStream(aFilename.c_str());
  boost::archive::xml_iarchive tInputArchive(tInFileStream);

  stage2.loadXML("tmp.xml", tInputArchive);

  EXPECT_EQ(stage.ops.size(), stage2.ops.size());

   // tStage.addOperation(new SingleOperation(tOperationName));
  
    //tStage.saveXML(filename);

//    tStage2.load(filename);
    
    //EXPECT_EQ(tCopy.mInputName ,tCopy2.mInputName);
    //EXPECT_EQ(tCopy.mOutputName ,tCopy2.mOutputName);
    
    
}




