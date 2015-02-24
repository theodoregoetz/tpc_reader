#ifndef TPC_DATA_MAP_HPP
#define TPC_DATA_MAP_HPP

#include <istream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "config.hpp"
#include "data_frame.hpp"

namespace tpc
{

using std::cerr;
using std::endl;
using std::ios;
using std::istream;
using std::ifstream;
using std::string;
using std::map;

typedef std::runtime_error system_error;

namespace fs = ::boost::filesystem;

inline
bool operator<(const DataFrameElementID& lhs,
               const DataFrameElementID& rhs)
{
    if ((lhs.asad     < rhs.asad   )  ||
        (lhs.asad    == rhs.asad    &&
         lhs.aget     < rhs.aget   )  ||
        (lhs.asad    == rhs.asad    &&
         lhs.aget    == rhs.aget    &&
         lhs.channel  < rhs.channel))
    {
        return true;
    }
    else
    {
        return false;
    }
}

struct PadID
{
    int ring;
    int id;
};

class DataMap
{

  private:
    map<DataFrameElementID,PadID> _map;

  public:
    DataMap() {}

    void load(const fs::path& filepath)
    {
        try
        {
            if (! fs::exists(filepath))
            {
                string err = "TPC map file (" + filepath.string() \
                           + ") does not exist\n";
                throw system_error(err);
            }
            fs::ifstream fin(filepath);
            DataFrameElementID key;
            PadID val;
            while (fin >> key.aget >> key.asad >> key.channel
                       >> val.ring >> val.id)
            {
                _map[key] = val;
            }
        }
        catch (exception& e)
        {
            cerr << "Exception reading in TPC map:\n"
                 << e.what() << endl;
            throw e;
        }
    }

    PadID pad_id(const DataFrameElementID& id)
    {
        return _map[id];
    }
};


} // namespace tpc

#endif // TPC_DATA_MAP_HPP

