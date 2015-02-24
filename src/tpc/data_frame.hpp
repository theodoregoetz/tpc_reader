#ifndef TPC_DATA_FRAME_HPP
#define TPC_DATA_FRAME_HPP

#include <inttypes.h> // uint32_t

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "config.hpp"
#include "detail/bswap.hpp"

namespace tpc
{

using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::istream;
using std::exception;


class DataFrameHeader
{
  private:
    vector<char> _data;
    union Word
    {
        char     c[4];
        uint32_t i;
    };
    int _get(size_t start, size_t size) const
    {
        Word w = {0,0,0,0};
        for (size_t i=0; i<size; i++)
        {
            w.c[i] = _data[start+size-i-1];
        }
        return w.i;
    }

  public:
    DataFrameHeader()
    : _data(frame_header_size)
    {}

    void read(istream& is)
    {
        is.read(&_data.front(),frame_header_size);
    }

    // meta_type always = 6
    int meta_type() const
    {
        return _get(0,1);
    }

    // whole frame in bytes
    int frame_size() const
    {
        return _get(1,3) * block_size;
    }

    // header in bytes
    int header_size() const
    {
        return _get(8,2) * block_size;
    }

    int event_index() const
    {
        return _get(22,4);
    }

    int read_offset() const
    {
        return _get(28,2);
    }

    int asad_index() const
    {
        return _get(27,1);
    }

    int last_cell(int aget) const
    {
        return _get(79 + 2 * aget, 2);
    }
};

class DataFrameBuffer : public vector<uint32_t>
{
  private:
    DataFrameHeader _header;
    void _bswap()
    {
        DataFrameBuffer::iterator x;
        for (x = this->begin(); x != this->end(); ++x)
        {
            *x = bswap32(*x);
        }
    }

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
                _bswap();
                return true;
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

                DataFrame::iterator elem = this->begin();
                DataFrameBuffer::const_iterator x;
                for (x = _buffer.begin(); x != _buffer.end(); ++x)
                {
                    elem->id.asad    = asad;
                    elem->id.aget    = *x >> 30 & 0x3;
                    elem->id.channel = *x >> 23 & 0x7f;
                    elem->cell       = *x >> 14 & 0x1ff;
                    elem->val        = *x & 0xfff;
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
