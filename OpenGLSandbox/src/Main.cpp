#include "Application.h"

#include "Log.h"

#include "scenes/EmissionScene.hpp"
#include "scenes/MultipleLightScene.hpp"
#include "scenes/ModelScene.hpp"
#include "scenes/DepthTestScene.hpp"
#include "scenes/ObjectOutliningScene.hpp"
#include "scenes/AlphaBlendingScene.hpp"
#include "scenes/PostProcessingScene.hpp"
#include "scenes/SkyboxScene.hpp"
#include "scenes/InstancedAsteroids.hpp"
#include "scenes/BlinnPhongBasic.hpp"
#include "scenes/BlinnPhongShadow.hpp"

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
	app.RegisterScene<Scenes::AlphaBlendingScene>( "Alpha-Blending" );
	app.RegisterScene<Scenes::BlinnPhongBasic>( "Blinn-Phong Basic" );
	app.RegisterScene<Scenes::BlinnPhongShadow>( "Blinn-Phong Shadow" );
	app.RegisterScene<Scenes::MultipleLightScene>( "Blinn-Phong Multiple light" );
	app.RegisterScene<Scenes::EmissionScene>( "Emission Directional light" );
	app.RegisterScene<Scenes::ObjectOutliningScene>( "Object outlining using stencil testing" );
	app.RegisterScene<Scenes::DepthTestScene>( "Color dependent on depth value" );
	app.RegisterScene<Scenes::PostProcessingScene>( "Post-processing using frame buffer" );
	app.RegisterScene<Scenes::SkyboxScene>( "Skybox and environment mapping" );
	app.RegisterScene<Scenes::ModelScene>( "Loading model using ASSIMP" );
	app.RegisterScene<Scenes::InstancedAsteroids>( "Instanced rotating asteroids" );

	app.PlayLoop();

	return 0;
}