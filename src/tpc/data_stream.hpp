#ifndef TPC_DATA_STREAM_HPP
#define TPC_DATA_STREAM_HPP

#include <istream>

namespace tpc
{

typedef std::char_traits<char> ctraits;
typedef std::basic_streambuf<char,ctraits> bcsbuf;

class DataStreamBuffer : bcsbuf

class DataIStream : std::basic_istream
{
  private:
  public:
    bool attach_file(const fs::path& filepath)
    {
        this = new std::ifstream(filepath);
    }

    ~DataIStream()
    {
        delete
    }
};

} // namespace tpc

#endif // TPC_DATA_STREAM_HPP
