#include "Application.hpp"

#include "scenes/PhongDirectionLightScene.hpp"
#include "scenes/PhongPointLightScene.hpp"
#include "scenes/PhongSpotLightScene.hpp"
#include "scenes/PhongMultipleLightScene.hpp"
#include "scenes/CustomObjectScene.hpp"
#include "scenes/DepthTestsScene.hpp"
#include "scenes/StencilTestsScene.hpp"
#include "scenes/AlphaBlendingScene.hpp"
#include "Log.h"

int main( void ) {
	Log::Init();

	Application app{ 1920, 1080, "OpenGL Sandbox" };
	app.Init();
	app.RegisterScene<Scenes::PhongDirectLightScene>( "Scene : Phong DirectLight" );
	app.RegisterScene<Scenes::PhongPointLightScene>( "Scene : Phong PointLight" );
	app.RegisterScene<Scenes::PhongSpotLightScene>( "Scene : Phong SpotLight" );
	app.RegisterScene<Scenes::PhongMultipleLightScene>( "Scene : Phong MultipleLight" );
	app.RegisterScene<Scenes::CustomObjectScene>( "Scene : Custom Object" );
	app.RegisterScene<Scenes::DepthTestsScene>( "Scene : Depth Tests Scene" );
	app.RegisterScene<Scenes::StencilTestsScene>( "Scene : Stencil Tests Scene" );
	app.RegisterScene<Scenes::AlphaBlendingScene>( "Scene : Alpha Blending Scene" );
	app.PlayLoop();
	return 0;
}