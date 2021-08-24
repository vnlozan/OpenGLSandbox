#include "Application.hpp"

#include "scenes/PhongDirectionLightScene.hpp"
#include "scenes/PhongPointLightScene.hpp"
#include "scenes/PhongSpotLightScene.hpp"
#include "scenes/PhongMultipleLightScene.hpp"
#include "scenes/CustomObjectScene.hpp"

int main( void ) {
	Application app{ 1920, 1080, "OpenGL Sandbox" };
	app.Init();
	app.RegisterScene<Scenes::PhongDirectLightScene>( "Scene : Phong DirectLight" );
	app.RegisterScene<Scenes::PhongPointLightScene>( "Scene : Phong PointLight" );
	app.RegisterScene<Scenes::PhongSpotLightScene>( "Scene : Phong SpotLight" );
	app.RegisterScene<Scenes::PhongMultipleLightScene>( "Scene : Phong MultipleLight" );
	app.RegisterScene<Scenes::CustomObjectScene>( "Scene : Custom Object" );
	app.PlayLoop();
	return 0;
}