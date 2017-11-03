#ifndef __HDF5_DUMP_HH
#define __HDF5_DUMP_HH
#include <string>

class mpid_data_stats;
void hdf5_dump(mpid_data_stats* mpi_stats, const std::string& name);

#endif
