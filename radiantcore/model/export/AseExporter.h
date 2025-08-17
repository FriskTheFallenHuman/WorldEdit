#pragma once

#include "imodel.h"
#include "ModelExporterBase.h"

namespace model
{

class AseExporter :
	public ModelExporterBase
{
public:
	IModelExporterPtr clone() override;

	const std::string& getDisplayName() const override;

	// Returns the uppercase file extension this exporter is suitable for
	const std::string& getExtension() const override;

	void exportToPath(const std::string& outputPath, const std::string& filename) override;

private:
	// Export the model file to the given stream
	void exportToStream(std::ostream& stream);
};

}
