#ifndef TPC_DATA_FRAME_HPP
#define TPC_DATA_FRAME_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "detail/bswap.hpp"

namespace tpc
{

using std::cerr;
using std::endl;
using std::uint32_t;
using std::array;
using std::string;
using std::istream;
using std::exception;

static const size_t nchannels = 68;
static const size_t ncells = 512;
static const size_t nagets = 4;
static const size_t frame_size = nchannels * ncells * nagets;

class DataFrameHeader
{
  private:
    static const size_t _size = 32;
    array<uint32_t,_size> _buffer;

  public:
    DataFrameHeader() {}
    void read(istream& is)
    {
        is.read((char*) _buffer.data(), _size * sizeof(uint32_t));
    }
    int nevent()
    {
        return int(   (bswap32(_buffer[5]) & 0xffff) << 16
                    |  bswap32(_buffer[6]) >> 16 );
    }
    int last_cell()
    {
        return int((bswap32(_buffer[21]) >> 8) & 0xffff);
    }
};

class DataFrameBuffer : public array<uint32_t,frame_size>
{
  private:
    DataFrameHeader _header;

  public:
    int nevent;
    int last_cell;
    bool read(istream& is)
    {
        try
        {
            if (is.good() && ! is.eof())
            {
                _header.read(is);
                this->nevent = _header.nevent();
                this->last_cell = _header.last_cell();

                is.read((char*) this->data(), frame_size * sizeof(uint32_t));
                for (auto& x : *this)
                {
                    x = bswap32(x);
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
                 << e.what();
            throw e;
        }
        return false;
    }
};

struct DataFrameElementID
{
    int aget;
    int channel;
    int cell;
};

struct DataFrameElement
{
    DataFrameElementID id;
    int val;
};

class DataFrame : public array<DataFrameElement,frame_size>
{
  private:
    DataFrameBuffer _buffer;

  public:
    bool read(istream& is)
    {
        try
        {
            if (_buffer.read(is))
            {
                auto elem = this->begin();
                for (const auto& x : _buffer)
                {
                    elem->id.aget    = x >> 30 & 0x3;
                    elem->id.channel = x >> 23 & 0x7f;
                    elem->id.cell    = x >> 14 & 0x1ff;
                    elem->val        = x & 0xfff;
                    ++elem;
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
                 << e.what();
            throw e;
        }
        return false;
    }

};

} // namespace tpc

#endif // TPC_DATA_FRAME_HPP
