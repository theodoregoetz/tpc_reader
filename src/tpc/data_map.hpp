#ifndef TPC_DATA_MAP_HPP
#define TPC_DATA_MAP_HPP

#include <istream>
#include <fstream>
#include <string>
#include <map>

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

enum class Side {LEFT, RIGHT};

inline
istream& operator>>(istream& is, Side& s)
{
    unsigned int side = 0;
    if (is >> side)
    {
        if (side > 1)
        {
            is.setstate(is.rdstate() | ios::failbit);
        }
        s = static_cast<Side>(side);
    }
    return is;
}

inline
bool operator<(const DataFrameElementID& lhs,
               const DataFrameElementID& rhs)
{
    if ((lhs.channel <  rhs.channel)   ||
        (lhs.channel == rhs.channel  &&
         lhs.cell    <  rhs.cell   )   ||
        (lhs.channel == rhs.channel  &&
         lhs.cell    == rhs.cell     &&
         lhs.aget    <  rhs.aget   ))
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
    Side side;
    int ring;
    int id;
};

class DataMap
{
  private:
    map<DataFrameElementID,PadID> _map;

  public:
    DataMap() {}

    void load(const string& filename)
    {
        try
        {
            ifstream fin(filename);
            DataFrameElementID key;
            PadID val;
            int c=0;
            while (fin >> key.channel >> key.cell >> key.aget
                       >> val.side >> val.ring >> val.id)
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

