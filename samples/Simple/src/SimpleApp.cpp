#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "DmtrUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SimpleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void SimpleApp::setup()
{
    

    DmtrUI::setup();
    
    DmtrUI::createLabel("This is a Label");
    DmtrUI::createSlider("mySlider", 0.0f, 100.0f, 50.0f);
    
    
}

void SimpleApp::mouseDown( MouseEvent event )
{
}

void SimpleApp::update()
{
}

void SimpleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::drawSolidCircle(getWindowCenter(), DmtrUI::pFloat["mySlider"]);
    
}

CINDER_APP( SimpleApp, RendererGl )
