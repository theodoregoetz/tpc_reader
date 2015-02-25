#include "unit_test.hpp"

#include <string>

#include <boost/filesystem.hpp>

#include "tpc/data_map.hpp"

BOOST_AUTO_TEST_SUITE( DataMap_test )

namespace fs = boost::filesystem;

using std::string;

using tpc::DataMap;
using tpc::PadID;
using tpc::DataFrameElementID;

BOOST_AUTO_TEST_CASE( DataMap_constructors_test )
{
    DataMap data_map;
}

BOOST_AUTO_TEST_CASE( DataMap_load_test )
{
    DataMap data_map;

    fs::path temp = fs::unique_path();
    fs::ofstream fout(temp);
    fout << "0 0 0 0 0\n"
            "0 0 1 0 1\n";
    fout.close();

    data_map.load(temp);

    fs::remove(temp);
}

BOOST_AUTO_TEST_CASE( DataMap_pad_id_test )
{
    DataMap data_map;

    fs::path temp = fs::unique_path();
    fs::ofstream fout(temp);
    fout << "0 0 0 0 0\n"
            "0 0 1 0 1\n";
    fout.close();

    data_map.load(temp);

    fs::remove(temp);

    DataFrameElementID id;
    id.aget = 0;
    id.asad = 0;
    id.channel = 0;
    PadID pid = data_map.pad_id(id);

    BOOST_CHECK_EQUAL(pid.ring, 0);
    BOOST_CHECK_EQUAL(pid.id,  0);

    id.channel = 1;
    pid = data_map.pad_id(id);

    BOOST_CHECK_EQUAL(pid.ring, 0);
    BOOST_CHECK_EQUAL(pid.id,  1);
}

BOOST_AUTO_TEST_SUITE_END()
