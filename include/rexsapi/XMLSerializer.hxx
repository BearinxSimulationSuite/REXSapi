/*
 * Copyright Schaeffler Technologies AG & Co. KG (info.de@schaeffler.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef REXSAPI_XML_SERIALIZER_HXX
#define REXSAPI_XML_SERIALIZER_HXX

#include <rexsapi/Xml.hxx>

namespace rexsapi
{
  class XMLFileSerializer
  {
  public:
    explicit XMLFileSerializer(std::filesystem::path file)
    : m_File{std::move(file)}
    {
      // TODO (lcf): check if the path exists and can be written to as the pugi xml writer does not do proper error
      // handling
    }

    void serialize(const pugi::xml_document& doc) const
    {
      doc.save_file(m_File.c_str(), "  ");
    }

  private:
    std::filesystem::path m_File;
  };


  class XMLStringSerializer
  {
  public:
    void serialize(const pugi::xml_document& doc)
    {
      std::stringstream stream;
      doc.save(stream, "  ");
      m_Model = stream.str();
    }

    const std::string& getModel() const&
    {
      return m_Model;
    }

  private:
    std::string m_Model;
  };
}

#endif
