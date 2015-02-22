#ifndef TPC_DATA_HPP
#define TPC_DATA_HPP

#include <iostream>
#include <array>
#include <vector>

#include "config.hpp"
#include "data_frame.hpp"
#include "data_map.hpp"
#include "detail/log.hpp"

namespace tpc
{

using std::istream;
using std::array;
using std::vector;
using std::exception;
using std::cerr;
using std::clog;
using std::endl;

class Data
{
  private:

    DataFrame _frame;
    DataMap _map;

    typedef array<int,ncells> ScalarDataPad;
    typedef vector<ScalarDataPad> ScalarDataRing;
    typedef vector<ScalarDataRing> ScalarData;

    ScalarData _adc;

    static
    void _clear_scalar_data(ScalarData& data)
    {
        for (auto& v : data)
        {
            for (auto& vv : v)
            {
                vv.fill(0);
            }
        }
    }

    static
    void _setup_scalar_data(ScalarData& data)
    {
        data.resize(nrings);
        for (int r = 0; r < nrings; r++)
        {
            data[r].resize(npads[r]);
        }
    }

  public:
    Data()
    {
        Data::_setup_scalar_data(_adc);
    }

    void load_map(const string& filename)
    {
        _map.load(filename);
    }

    void clear()
    {
        Data::_clear_scalar_data(_adc);
    }

    bool read(istream& is)
    {
        try
        {
            // currently reading in only one frame.
            // This is only a single asad. Should
            // read in 31 frames for a complete event
            if (_frame.read(is))
            {
                for (const auto& elem : _frame)
                {
                    const PadID& pad = _map.pad_id(elem.id);
                    _adc[pad.ring][pad.id][elem.cell] = elem.val;
                }
            }
            else
            {
                return false;
            }
            return true;
        }
        catch (exception& e)
        {
            cerr << "Exception reading raw TPC data:\n"
                 << e.what() << endl;
            throw e;
        }
        return false;
    }

    ScalarDataPad adc(size_t ring, size_t pad)
    {
        return _adc[ring][pad];
    }
};

} // namespace tpc

#endif // TPC_DATA_HPP
