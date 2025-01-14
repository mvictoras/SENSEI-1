#ifndef sensei_VTKDataAdaptor_h
#define sensei_VTKDataAdaptor_h

#include "DataAdaptor.h"
#include <vtkSmartPointer.h>

class vtkDataObject;

namespace sensei
{
/// @brief DataAdaptor for a vtkDataObject.
///
/// sensei::VTKDataAdaptor is a simple implementation of sensei::DataAdaptor
/// that provides the sensei::DataAdaptor on a vtkDataObject. To use this data
/// adaptor for codes that produce a vtkDataObject natively. Once simply passes
/// the vtkDataObject instance to this class and one then connect it to the
/// sensei::AnalysisAdators.
///
/// If the DataObject is a composite-dataset, the first non-null block on the
/// current rank is assumed to the representative block for answering all
/// queries.
class VTKDataAdaptor : public DataAdaptor
{
public:
  static VTKDataAdaptor *New();
  senseiTypeMacro(VTKDataAdaptor, DataAdaptor);

  /// @brief Set the data object for current timestep.
  ///
  /// This method must be called to the set the data object for
  /// the current timestep.
  ///
  /// @param[in] meshName name of the mesh represented in the object
  /// @param[in] mesh the mesh or nullptr if this rank has no data
  void SetDataObject(const std::string &meshName, vtkDataObject *mesh);

  /// @breif Get the data object for the current time step
  ///
  /// This method returns the mesh associated with the passed in
  /// name. It returns zero if the mesh is present and non-zero
  /// if it is not.
  ///
  /// @param[in] meshName the name of the mesh to get
  /// @param[out] mesh a reference to store a pointer to the named mesh
  /// @returns zero if the named mesh is present, non zero if it was not
  int GetDataObject(const std::string &meshName, vtkDataObject *&dobj);

  /// @breif Gets the number of meshes a simulation can provide
  ///
  /// The caller passes a reference to an integer variable in the first
  /// argument upon return this variable contains the number of meshes the
  /// simulation can provide. If successfull the method returns 0, a non-zero
  /// return indicates an error occurred.
  ///
  /// param[out] numMeshes an integer variable where number of meshes is stored
  /// @returns zero if successful, non zero if an error occurred
  int GetNumberOfMeshes(unsigned int &numMeshes) override;

  /// @brief Get metadata of the i'th mesh
  ///
  /// The caller passes the integer id of the mesh for which the metadata is
  /// desired, and a pointer to a MeshMetadata instance  where the metadata is
  /// stored.
  ///
  /// @param[in] id index of the mesh to access
  /// @param[out] metadata a pointer to instance where metadata is stored
  /// @returns zero if successful, non zero if an error occurred
  int GetMeshMetadata(unsigned int id, MeshMetadataPtr &metadata) override;

  /// @brief Return the data object with appropriate structure.
  ///
  /// This method will return a data object of the appropriate type. The data
  /// object can be a vtkDataSet subclass or a vtkCompositeDataSet subclass.
  /// If \c structure_only is set to true, then the geometry and topology
  /// information will not be populated. For data adaptors that produce a
  /// vtkCompositeDataSet subclass, passing \c structure_only will still produce
  /// appropriate composite data hierarchy.
  ///
  /// @param[in] meshName the name of the mesh to access (see GetMeshName)
  /// @param[in] structure_only When set to true (default; false) the returned mesh
  ///            may not have any geometry or topology information.
  /// @param[out] a reference to a pointer where a new VTK object is stored
  /// @returns zero if successful, non zero if an error occurred
  int GetMesh(const std::string &meshName, bool structure_only,
    vtkDataObject *&mesh) override;

  using sensei::DataAdaptor::GetMesh;

  /// @brief Adds the specified field array to the mesh.
  ///
  /// This method will add the requested array to the mesh, if available. If the
  /// array was already added to the mesh, this will not add it again. The mesh
  /// should not be expected to have geometry or topology information.
  ///
  /// @param[in] mesh the VTK object returned from GetMesh
  /// @param[in] meshName the name of the mesh on which the array is stored
  /// @param[in] association field association; one of
  ///            vtkDataObject::FieldAssociations or vtkDataObject::AttributeTypes.
  /// @param[in] arrayName name of the array
  /// @returns zero if successful, non zero if an error occurred
  int AddArray(vtkDataObject* mesh, const std::string &meshName,
    int association, const std::string &arrayName) override;

  /// @brief Release data allocated for the current timestep.
  ///
  /// Releases the data allocated for the current timestep. This is expected to
  /// be called after each time iteration.
  ///
  /// @returns zero if successful, non zero if an error occurred
  int ReleaseData() override;

protected:
  VTKDataAdaptor();
  ~VTKDataAdaptor();

private:
  VTKDataAdaptor(const VTKDataAdaptor&); // Not implemented.
  void operator=(const VTKDataAdaptor&); // Not implemented.

  struct InternalsType;
  InternalsType *Internals;

};

}

#endif
