#ifndef HistogramInternals_h
#define HistogramInternals_h

// #define SENSEI_DEBUG 1

class vtkUnsignedCharArray;
class vtkDataArray;

#include <mpi.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <limits>

namespace sensei
{
/// Distributed MPI+X paralllel histogram
/** Computes a histogram on multiple data blocks with one array per block.
 * CUDA will be used for the calculations if ENABLE_CUDA is defined during the
 * build, otherwise the CPU is used. The data arrays must have only one
 * component.
 *
 * Call the methods in the following order:
 *
 * Initialize
 * AddLocalData (once per local data block)
 * ComputeHistogram
 * GetHistogram
 * Clear
 *
 * All methods return 0 if successful.
 */
class HistogramInternals
{
public:
    HistogramInternals() = delete;

    HistogramInternals(MPI_Comm comm, int deviceId, int numberOfBins) :
      Comm(comm),
      DeviceId(deviceId),
      NumberOfBins(numberOfBins),
      Min(std::numeric_limits<double>::max()),
      Max(std::numeric_limits<double>::lowest()),
      Width(1.0)
    {}

    ~HistogramInternals();

    /** set up for the calculation */
    int Initialize();

    /** add block local contributions */
    int AddLocalData(vtkDataArray *da, vtkUnsignedCharArray *ghostArray);

    /** compute the histogram. this call uses MPI collectives, all ranks must
     * participate */
    int ComputeHistogram();

    /// return the computed histogram, only valid on MPI rank 0
    int GetHistogram(int &nBins, double &binMin, double &binMax,
      double &binWidth, std::vector<unsigned int> &histogram);

    /** free all cached memory and reset all internal parameters */
    int Clear();

private:
    /** compute the global min and max across all MPI ranks and blocks*/
    int ComputeRange();

    /** initialize the histogram, must be called after ComputeGlobalRange */
    int InitializeHistogram();

    /** compute the local histgrams */
    int ComputeLocalHistogram();

    /** Apply a reduction to locally computed histograms across all ranks.
     * Result is valid only on rank 0 */
    int FinalizeHistogram();

private:
  MPI_Comm Comm;
  int DeviceId;
  int NumberOfBins;
  double Min;
  double Max;
  double Width;
  std::map<vtkDataArray*, std::shared_ptr<void>> DataCache;
  std::map<vtkDataArray*, std::shared_ptr<unsigned char>> GhostCache;
  std::shared_ptr<unsigned int> Histogram;
};

}
#endif
