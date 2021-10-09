#include "Application.hpp"

#include "Log.h"

#include "scenes/EmissionScene.hpp"
#include "scenes/PhongMultipleLightScene.hpp"
#include "scenes/ModelScene.hpp"
#include "scenes/DepthTestScene.hpp"
#include "scenes/ObjectOutliningScene.hpp"
#include "scenes/AlphaBlendingScene.hpp"
#include "scenes/PostProcessingScene.hpp"
#include "scenes/SkyboxScene.hpp"
#include "scenes/InstancingScene.hpp"
#include "scenes/InstancingAsteroids.hpp"

/*
TODO
Model<->camera Distance
Fix defaults
Fixed framerate
*/
int main( void ) {
	Log::Init();

	Application app{ 1920, 1080, "OpenGL Sandbox" };
	
	app.Init();

	app.RegisterScene<Scenes::ModelScene>( "Demo: Loading model using ASSIMP" );
	app.RegisterScene<Scenes::PhongMultipleLightScene>( "Demo: Phong Multiple light" );
	app.RegisterScene<Scenes::EmissionScene>( "Demo: Emission Directional light" );
	app.RegisterScene<Scenes::ObjectOutliningScene>( "Demo: Object outlining using stencil testing" );
	app.RegisterScene<Scenes::DepthTestScene>( "Demo: Color dependent on depth value" );
	app.RegisterScene<Scenes::PostProcessingScene>( "Demo: Post-processing using frame buffer" );
	app.RegisterScene<Scenes::SkyboxScene>( "Demo: Skybox and environment mapping" );
	app.RegisterScene<Scenes::AlphaBlendingScene>( "Demo: Alpha blending" );
	app.RegisterScene<Scenes::InstancingScene>( "Demo: Basic instancing" );
	app.RegisterScene<Scenes::InstancingAsteroids>( "Demo: Asteroids instancing" );

	app.PlayLoop();

	return 0;
}