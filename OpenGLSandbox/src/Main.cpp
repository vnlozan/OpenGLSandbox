#include "Application.hpp"

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
	app.RegisterScene<Scenes::AlphaBlendingScene>( "Demo: Alpha-Blending" );
	app.RegisterScene<Scenes::BlinnPhongBasic>( "Demo: Blinn-Phong Basic" );
	app.RegisterScene<Scenes::BlinnPhongShadow>( "Demo: Blinn-Phong Shadow" );
	app.RegisterScene<Scenes::MultipleLightScene>( "Demo: Blinn-Phong Multiple light" );
	app.RegisterScene<Scenes::EmissionScene>( "Demo: Emission Directional light" );
	app.RegisterScene<Scenes::ObjectOutliningScene>( "Demo: Object outlining using stencil testing" );
	app.RegisterScene<Scenes::DepthTestScene>( "Demo: Color dependent on depth value" );
	app.RegisterScene<Scenes::PostProcessingScene>( "Demo: Post-processing using frame buffer" );
	app.RegisterScene<Scenes::SkyboxScene>( "Demo: Skybox and environment mapping" );
	app.RegisterScene<Scenes::ModelScene>( "Demo: Loading model using ASSIMP" );
	app.RegisterScene<Scenes::InstancedAsteroids>( "Demo: Instanced rotation asteroids" );

	app.PlayLoop();

	return 0;
}