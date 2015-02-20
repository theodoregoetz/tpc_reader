#ifndef TPC_DATA_HPP
#define TPC_DATA_HPP

#include <iostream>
#include <array>
#include <vector>

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

static const size_t nrings = 32;
static const array<size_t,nrings> npads{
     48,  48,  72,  96, 120, 144, 168, 192, 216, 240,
    208, 218, 230, 214, 212, 214, 220, 224, 232, 238,
    244, 232, 218, 210, 206, 202, 200, 196, 178, 130,
    108,  90 };
static const size_t nsamples = ncells;

class Data
{
  private:

    DataFrame _frame;
    DataMap _map;

    typedef array<int,nsamples> ScalarDataPad;
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

    bool read(istream& is, size_t nframes = nsamples)
    {
        try
        {
            PadID pad;
            for (int i = 0; i < nframes; i++)
            {
                if (_frame.read(is))
                {
                    for (const auto& elem : _frame)
                    {
                        pad = _map.pad_id(elem.id);
                        _adc[pad.ring][pad.id][elem.cell] = elem.val;
                    }
                }
                else
                {
                    return false;
                }
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
