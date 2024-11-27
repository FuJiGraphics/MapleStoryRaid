#include <VegaEngine2.h>
#include "Editor2D.h"

using namespace std;

class VegaEditor : public fz::System
{
public:
	VegaEditor()
		: System(1920, 1080, "VegaEngine2")
	{
		this->PushLayer(LayerGenerator->Take<fz::Editor2D>("Editor2D"));
	}
	virtual ~VegaEditor()
	{
		// Empty
	}

private:

};

fz::System* fz::CreateSystem()
{
	return new VegaEditor();
}