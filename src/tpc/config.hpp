#ifndef TPC_CONFIG_HPP
#define TPC_CONFIG_HPP

namespace tpc
{

// number of ASAD boards
static const size_t nasads = 31;

// number of AGET chips on each ASAD board
static const size_t nagets = 4;

// number of channels read out by one AGET chip
static const size_t nchannels = 68;

// total number of raw data points per timing cell
static const size_t frame_length = nasads * nagets * nchannels;

// number of timing cells (buckets) in the ADC window
static const size_t ncells = 512;

// block size for data transfers
// 1 block = 64 bytes
static const size_t block_size = 64;

// frame header size in bytes
static const size_t frame_header_size = 2 * block_size;

// number of rings of pads on the TPC
static const size_t nrings = 32;

// number of pads in each ring starting from the center
static const size_t npads[nrings] = {
     48,  48,  72,  96, 120, 144, 168, 192, 216, 240,
    208, 218, 230, 214, 212, 214, 220, 224, 232, 238,
    244, 232, 218, 210, 206, 202, 200, 196, 178, 130,
    108,  90 };

} // namespace tpc

#endif // TPC_CONFIG_HPP
