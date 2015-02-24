#ifndef TPC_DATA_HPP
#define TPC_DATA_HPP

#include <iostream>
#include <vector>

#include "config.hpp"
#include "data_frame.hpp"
#include "data_map.hpp"
#include "detail/log.hpp"

namespace tpc
{

using std::istream;
using std::vector;
using std::exception;
using std::cerr;
using std::clog;
using std::endl;

class Data
{
  public:

    typedef vector<int> ScalarDataPad;
    typedef vector<ScalarDataPad> ScalarDataRing;
    typedef vector<ScalarDataRing> ScalarData;

  private:

    DataFrame _frame;
    DataMap _map;
    ScalarData _adc;

    static
    void _clear_scalar_data(ScalarData& data)
    {
        ScalarData::iterator v;
        for (v = data.begin(); v != data.end(); ++v)
        {
            ScalarDataRing::iterator vv;
            for (vv = v->begin(); vv != v->end(); ++vv)
            {
                vv->assign(vv->size(),0);
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
            ScalarDataRing::iterator v;
            for (v = data[r].begin(); v != data[r].end(); ++v)
            {
                v->resize(ncells);
            }
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
                DataFrame::const_iterator elem;
                for (elem=_frame.begin(); elem != _frame.end(); ++elem)
                {
                    const PadID& pad = _map.pad_id(elem->id);
                    _adc[pad.ring][pad.id][elem->cell] = elem->val;
                }
                return true;
            }
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
