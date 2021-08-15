#include "Application.h"

#include "scenes/PhongDirectionLightScene.h"
#include "scenes/PhongPointLightScene.h"
#include "scenes/PhongSpotLightScene.h"
#include "scenes/PhongMultipleLightScene.h"


int main( void ) {
	Application app{ 1920, 1080, "OpenGL Sandbox" };
	app.Init();
	app.RegisterScene<Scenes::PhongDirectLightScene>( "Scene : Phong DirectLight" );
	app.RegisterScene<Scenes::PhongPointLightScene>( "Scene : Phong PointLight" );
	app.RegisterScene<Scenes::PhongSpotLightScene>( "Scene : Phong SpotLight" );
	app.RegisterScene<Scenes::PhongMultipleLightScene>( "Scene : Phong MultipleLight" );
	app.PlayLoop();
	return 0;
}