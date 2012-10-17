// Class SdkMesh is the only interface to deal with CDXUTSDKmesh
// SdkMesh::load is used to initialize VboMesh from CDXUTSDKmesh

#pragma once

#include "cinder/DataSource.h"

class CDXUTSDKMesh;

namespace cinder { namespace dx11{

class SdkMesh
{
public:
	SdkMesh( DataSourceRef dataSource, bool includeUVs = true );
	void load( uint32_t iMesh, class VboMesh* target ) const;

private:

	std::shared_ptr<CDXUTSDKMesh> mSdkMesh;
};

}}