#include "Application.hpp"

#include "Log.h"

#include "scenes/EmissionScene.hpp"
#include "scenes/PhongMultipleLightScene.hpp"
#include "scenes/ModelScene.hpp"
#include "scenes/DepthTestScene.hpp"
#include "scenes/StencilTestScene.hpp"
#include "scenes/AlphaBlendingScene.hpp"
#include "scenes/FrameBufferScene.hpp"
#include "scenes/SkyboxScene.hpp"
#include "scenes/InstancingScene.hpp"

int main( void ) {
	Log::Init();

	Application app{ 1920, 1080, "OpenGL Sandbox" };
	
	app.Init();

	/* Add Vertex Normals, Triangle Normals */
	app.RegisterScene<Scenes::ModelScene>( "Demo: Custom Object using ASSIMP" );
	app.RegisterScene<Scenes::PhongMultipleLightScene>( "Demo: Phong Multiple light" );
	app.RegisterScene<Scenes::EmissionScene>( "Demo: Emission Directional light" );
	app.RegisterScene<Scenes::StencilTestScene>( "Demo: Stencil use" );
	app.RegisterScene<Scenes::DepthTestScene>( "Demo: Color dependent on depth value" );
	app.RegisterScene<Scenes::FrameBufferScene>( "Demo: Post-processing using frameBuffer" );
	app.RegisterScene<Scenes::SkyboxScene>( "Demo: Skybox and environment mapping" );
	app.RegisterScene<Scenes::AlphaBlendingScene>( "Demo: Alpha blending" );
	app.RegisterScene<Scenes::InstancingScene>( "Demo: Basic instancing" );

	app.PlayLoop();

	return 0;
}