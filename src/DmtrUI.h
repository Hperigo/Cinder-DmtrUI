#include "cinder/Xml.h"


namespace dmtrUI {

	using namespace ci;
	using namespace ci::app;
	using namespace std;
	using namespace cinder::gl;

	enum elementType {
		SLIDER, SLIDERINT, TOGGLE, LABEL
	};


	
	class element {
	public:
		elementType tipo;
		float *_val;
		string nome;
		Rectf rect;
		Color cor;
		void init() {

		}

		void draw() {
			color(cor);
			drawSolidRect(rect);
			float x2 = lmap<float>(*_val, 0, 1, rect.x1, rect.x2);
			color(0,0,0,.4);
			drawSolidRect(Rectf(rect.x1, rect.y1, x2, rect.y2));
			drawString(nome, rect.getUpperLeft() + vec2(9,11), ColorA(0,0,0,1) );
			drawString(nome, rect.getUpperLeft() + vec2(8,10), ColorA(1,1,1,1.0));
		}

		void checkMouse(vec2 mouse) {
			*_val = (mouse.x - rect.getX1()) / (float) rect.getWidth();
		}
	};


	map <string, float> pFloat;
	vector <element> elements;
	vec2 flow;


	void mouseDown( MouseEvent event )
	{
		for (auto & e : elements) {
			if (e.rect.contains(event.getPos())) {
				e.checkMouse(event.getPos());
			}
		}
	}

	void mouseDrag( MouseEvent event )
	{
		for (auto & e : elements) {
			if (e.rect.contains(event.getPos())) {
				e.checkMouse(event.getPos());
			}
		}
	}

	void draw() {
		for (auto & e : elements) {
			e.draw();
		}
	}

	void setup( const ci::app::WindowRef& window = ci::app::getWindow()) {
		static vector<signals::Connection> sWindowConnections;
		sWindowConnections = {
			window->getSignalMouseDown().connect( mouseDown ),
			window->getSignalMouseDrag().connect( mouseDrag ),
			window->getSignalDraw().connect ( draw ), 
			//			window->getSignalMouseUp().connect( mouseUp ),
			//			window->getSignalMouseMove().connect( mouseMove ),
			//			window->getSignalMouseWheel().connect( mouseWheel ),
			//			window->getSignalKeyDown().connect( keyDown ),
			//			window->getSignalKeyUp().connect( keyUp ),
			//			window->getSignalResize().connect( resize ),
		};
	}





	void createSlider(string nome, float min, float max, float def) {
		int hue = int(flow.x + flow.y/6.0)%255;
		element te; // temp element
		te.tipo = SLIDER;
		te.nome = nome;
		te.cor = Color ( CM_HSV, hue/255.0,1, 1 );
		te.rect = Rectf(flow.x, flow.y, flow.x + 240, flow.y + 20);
		te._val = &dmtrUI::pFloat[te.nome];
		elements.push_back(te);
		flow.y += 25;
	}

	void save(string filename) {
		XmlTree saveUI ("ui", "");
		for (auto & e : elements) {
			saveUI.push_back( XmlTree(e.nome, to_string(*e._val)));
		}
		saveUI.write( writeFile(filename) );
	}

	void load(string filename) {
		XmlTree doc ( loadFile (filename) );
		XmlTree ui =  doc.getChild("ui");
		for (auto & e : elements) {
			*e._val = (ui.getChild(e.nome).getValue<float>());
		}

	}

	/*
	 
	 static vector<signals::Connection> sWindowConnections;

	 void connectWindow( ci::app::WindowRef window )
	 {
	 sWindowConnections = {
		window->getSignalMouseDown().connect( mouseDown ),
		window->getSignalMouseUp().connect( mouseUp ),
		window->getSignalMouseDrag().connect( mouseDrag ),
		window->getSignalMouseMove().connect( mouseMove ),
		window->getSignalMouseWheel().connect( mouseWheel ),
		window->getSignalKeyDown().connect( keyDown ),
		window->getSignalKeyUp().connect( keyUp ),
		window->getSignalResize().connect( resize ),
	 };
	 }*/

}; // end namespace;


