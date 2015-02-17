#ifndef TPC_DATA_HPP
#define TPC_DATA_HPP

#include <iostream>
#include <array>
#include <vector>

#include "data_frame.hpp"
#include "data_map.hpp"

namespace tpc
{

using std::array;
using std::vector;
using std::exception;
using std::cerr;
using std::endl;

static const size_t nrings = 31;
static const array<size_t,nrings> npads{
    128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,
    128,128,128,128,128,128,128,128,128,128,
    128, };

class Data
{
  private:

    DataFrame _frame;
    DataMap _map;

    typedef vector<int> ScalarData;
    typedef vector<ScalarData> ScalarDataRing;
    typedef vector<ScalarDataRing> ScalarDataSide;

    map<Side,ScalarDataSide> _adc;
    map<Side,ScalarDataSide> _tdc;

    static
    void _clear_scalar_data(map<Side,ScalarDataSide>& data_map)
    {
        for (auto side : {Side::LEFT,Side::RIGHT})
        {
            for (auto& vv : data_map[side])
            {
                for (auto& v : vv)
                {
                    v.clear();
                }
            }
        }
    }

    static
    void _setup_scalar_data(map<Side,ScalarDataSide>& data_map)
    {
        data_map[Side::LEFT ] = ScalarDataSide(nrings);
        data_map[Side::RIGHT] = ScalarDataSide(nrings);

        for (auto side : {Side::LEFT,Side::RIGHT})
        {
            data_map[side] = ScalarDataSide(nrings);

            for (int r = 0; r < nrings; r++)
            {
                data_map[side][r].resize(npads[r]);
            }
        }
    }

  public:
    Data()
    {
        Data::_setup_scalar_data(_adc);
        Data::_setup_scalar_data(_tdc);
    }

    void load_map(const string& filename)
    {
        _map.load(filename);
    }

    void clear()
    {
        Data::_clear_scalar_data(_adc);
        Data::_clear_scalar_data(_tdc);
    }

    bool read(ifstream& fin)
    {
        try
        {
            if (_frame.read(fin))
            {
                this->clear();

                PadID pad;
                for (const auto& elem : _frame)
                {
                    pad = _map.pad_id(elem.id);
                    _adc[pad.side][pad.ring][pad.id].push_back(elem.val);
                }
                return true;
            }
            else
            {
                return false;
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

    ScalarData adc(Side side, size_t ring, size_t pad)
    {
        return _adc[side][ring][pad];
    }

    ScalarData tdc(Side side, size_t ring, size_t pad)
    {
        return _tdc[side][ring][pad];
    }
};

} // namespace tpc

#endif // TPC_DATA_HPP
