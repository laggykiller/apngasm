#include "specwriter.h"
#include "priv/specwriterimpl.h"
#include "../apngasm.h"
#include <boost/filesystem/operations.hpp>

namespace apngasm {
  namespace spec {

    namespace {
      bool isSeparator(const char c)
      {
        return c == '/' || c == '\\';
      }

      const boost::filesystem::path createAbsolutePath(const std::string& path)
      {
        const boost::filesystem::path oldPath = boost::filesystem::current_path();
        boost::filesystem::path result = path;
        boost::filesystem::current_path(result.parent_path());
        result = boost::filesystem::current_path();
        boost::filesystem::current_path(oldPath);
        return result;
      }
      const std::string createRelativeDir(const std::string& from, const std::string& to)
      {
        const boost::filesystem::path fromPath = createAbsolutePath(from);
        const boost::filesystem::path toPath = createAbsolutePath(to);

        // Other drive.
        if(fromPath.root_name() != toPath.root_name())
          return fromPath.string() + "/";

        std::string fromDir = fromPath.string() + "/";
        std::string toDir = toPath.string() + "/";

        {
          const int count = std::min(fromDir.length(), toDir.length());
          int find = -1;
          for(int i = 0;  i < count;  ++i)
          {
            const char fromChar = fromDir.at(i);
            const char toChar = toDir.at(i);

            if(fromChar == toChar)
            {
              if( isSeparator(fromChar) )
                find = i;
            }
            else
              break;
          }
          if(find != -1)
          {
            fromDir = fromDir.substr(find + 1);
            toDir = toDir.substr(find + 1);
          }
        }

        std::string result = "";
        if(!fromDir.empty())
        {
          const int count = fromDir.length();
          bool beforeIsSeparator = true;
          for(int i = 0;  i < count;  ++i)
          {
            const char currentChar = fromDir.at(i);
            if( isSeparator(currentChar) )
            {
              if( !beforeIsSeparator )
              {
                result += "../";
                beforeIsSeparator = true;
              }
            }
            else
            {
              beforeIsSeparator = false;
            }
          }
        }
        result += toDir;

        return result;
      }
    } // unnnamed namespace

    // Initialize SpecWriter object.
    SpecWriter::SpecWriter(const APNGAsm *pApngasm, const listener::ISaveListener* pSaveListener)
      : _pApngasm(pApngasm)
      , _pSaveListener(pSaveListener)
    {
      // nop
    }

    // Write APNGAsm object to json file.
    // Return true if write succeeded.
    bool SpecWriter::writeJson(const std::string& filePath, const std::string& imageDir) const
    {
      if( !_pApngasm )
        return false;

      priv::JsonSpecWriterImpl impl(_pApngasm, _pSaveListener);
      return impl.write(filePath, createRelativeDir(filePath, imageDir + "/"));
    }

    // Write APNGAsm object to xml file.
    // Return true if write succeeded.
    bool SpecWriter::writeXml(const std::string& filePath, const std::string& imageDir) const
    {
      if( !_pApngasm )
        return false;

      priv::XmlSpecWriterImpl impl(_pApngasm, _pSaveListener);
      return impl.write(filePath, createRelativeDir(filePath, imageDir + "/"));
    }

  } // namespace spec
} // namespace apngasm
