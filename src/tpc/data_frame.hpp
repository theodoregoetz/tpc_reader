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
using std::vector;
using std::string;
using std::istream;
using std::exception;

static const size_t nagets = 4;
static const size_t nasads = 31;
static const size_t nchannels = 68;

static const size_t frame_length = nagets * nasads * nchannels;

static const size_t ncells = 512;

static const size_t block_size = 64; // 1 block = 64 bytes
static const size_t frame_header_size = 2 * block_size; // bytes

union Word
{
    array<char,4> c;
    uint32_t      i;
};

class DataFrameHeader
{
  private:
    array<char,frame_header_size> _data;

  public:
    DataFrameHeader() {}

    void read(istream& is)
    {
        is.read(&_data.front(),frame_header_size);
    }

    // meta_type always = 6
    int meta_type() const
    {
        return Word{_data[0],0,0,0}.i;
    }

    // whole frame in bytes
    int frame_size() const
    {
        return Word{_data[3],_data[2],_data[1],0}.i * block_size;
    }

    // header in bytes
    int header_size() const
    {
        return Word{_data[9],_data[8],0,0}.i * block_size;
    }

    int event_index() const
    {
        return Word{_data[25],_data[24],_data[23],_data[22]}.i;
    }

    int read_offset() const
    {
        return Word{_data[29],_data[28],0,0}.i;
    }

    int asad_index() const
    {
        return Word{_data[27],0,0,0}.i;
    }

    int last_cell(int aget) const
    {
        char i0 = 79 + aget * 2;
        char i1 = 80 + aget * 2;
        return Word{i1,i0,0,0}.i;
    }
};

class DataFrameBuffer : public vector<uint32_t>
{
  private:
    DataFrameHeader _header;

  public:
    bool read(istream& is)
    {
        try
        {
            if (is.good() && ! is.eof())
            {
                _header.read(is);
                size_t frame_data_size = _header.frame_size() - _header.header_size();
                size_t frame_data_words = frame_data_size / sizeof(uint32_t);
                this->resize(frame_data_words);
                is.read((char*)this->data(), frame_data_size);
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

    const
    DataFrameHeader& header()
    {
        return _header;
    }
};

struct DataFrameElementID
{
    int aget;
    int asad;
    int channel;
};

struct DataFrameElement
{
    DataFrameElementID id;
    int cell;
    int val;
};

class DataFrame : public vector<DataFrameElement>
{
  private:
    DataFrameBuffer _buffer;

  public:
    DataFrame() {}

    bool read(istream& is)
    {
        try
        {
            if (_buffer.read(is))
            {
                int asad    = _buffer.header().asad_index();

                this->resize(_buffer.header().frame_size() / sizeof(uint32_t));

                auto elem = this->begin();
                for (const auto& x : _buffer)
                {
                    elem->id.asad    = asad;
                    elem->id.aget    = x >> 30 & 0x3;
                    elem->id.channel = x >> 23 & 0x7f;
                    elem->cell       = x >> 14 & 0x1ff;
                    elem->val        = x & 0xfff;
                    if (elem->id.asad < nasads &&
                        elem->id.aget < nagets &&
                        elem->id.channel < nchannels &&
                        elem->cell < ncells)
                    {
                        ++elem;
                    }
                }
                this->erase(elem,this->end());
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

    const
    DataFrameHeader& header()
    {
        return _buffer.header();
    }

    const
    DataFrameBuffer& buffer()
    {
        return _buffer;
    }
};

} // namespace tpc

#endif // TPC_DATA_FRAME_HPP
