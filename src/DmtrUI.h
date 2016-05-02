#include "cinder/Xml.h"
#include "cinder/gl/TextureFont.h"

namespace DmtrUI {

	using namespace ci;
	using namespace ci::app;
	using namespace std;
	using namespace cinder::gl;


	gl::FboRef			mFbo;
	Rectf coluna;
	bool redraw = true;
	bool useFbo = false;

	map <string, float> 		pFloat;
	map <string, int>  	 	pInt;
	map <string, bool>	 	pBool;
	map <string, string> 	pString;

	vector <Color> cores;
	vector <Color> coresOver;

	Font fonte;
	gl::TextureFontRef mFont;

	int sliderWidth = 200;
	int sliderHeight = 25;
	int sliderMargin = 5;

	enum elementType {
		SLIDER, SLIDERINT, TOGGLE, LABEL, RADIO
	};

	class element {
	public:
		elementType tipo;
		float 	*_val;
		int   	*_valInt;
		bool	  	*_valBool;
		string	*_valString;
		string nome;
		Rectf rect;
		Color cor;
		ColorA cor2 = ColorA(0,0,0,.4);
		float		min = 0;
		float		max = 1;
		float		def = .5;

		void init(string n, elementType t) {
			nome = n;
			tipo = t;
			cor = cores[0];
			cor2 = coresOver[0];
			if (tipo == SLIDER) {
				_val = &pFloat[nome];
			}
			else if (tipo == SLIDERINT) {
				_valInt = &pInt[nome];
				cor = cores[5];
				cor2 = coresOver[5];
			}
			else if (tipo == TOGGLE) {
				_valBool = &pBool[nome];
			}
			else if (tipo == RADIO) {
				_valString = &pString[nome];
			}

		}

		void draw() {
			color(cor);
			drawSolidRect(rect);
			float x2;
			if (tipo == SLIDER) {
				x2 = lmap<float>(*_val, min, max, 0, rect.x2 - rect.x1);
			}
			else if (tipo == SLIDERINT) {
				x2 = lmap<int>(*_valInt, min, max, 0, rect.x2 - rect.x1);
			}
			color(cor2);
			drawSolidRect(Rectf(rect.x1, rect.y1, rect.x1 + x2, rect.y2));
			//drawString(nome, rect.getUpperLeft() + vec2(9,11), ColorA(0,0,0,1) );

			color(Color(1,1,1));
			//fonte.
			mFont->drawString((nome), rect.getUpperLeft() + vec2(8,17));
			//drawString(nome, rect.getUpperLeft() + vec2(8,10), ColorA(1,1,1,1.0));
		}

		void checkMouse(vec2 mouse) {
//			cout << mouse << endl;
//			cout << rect << endl;
//			cout << "----" << endl;
			if (tipo == SLIDER) {
				*_val = lmap<float>(mouse.x , rect.getX1(), rect.getX2(), min, max);
			} else if (tipo == SLIDERINT) {
				*_valInt = lmap<int>(mouse.x , rect.getX1(), rect.getX2(), min, max);
			}
			else if (tipo == TOGGLE) {
				*_valBool = !_valBool;
			}
			redraw = true;
		}
	};



	vector <element> elements;
	vec2 flow = vec2(20,20);

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

	void keyDown( KeyEvent event ) {
		char key = event.getChar();
		if (key == 'a') {
			useFbo = !useFbo;
			redraw = true;
		}
		if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' ||
			key == '8' || key == '9' || key == '0'
			) {
			string nome = string(1,key) + ".xml";
			if (event.isMetaDown()) {
				cout << "save: " + nome << endl;
				save(nome);
			} else {
				cout << "load: " + nome << endl;
				load(nome);
			}
		}

	}
	void keyUp( KeyEvent event ) {

	}


	void setup( const ci::app::WindowRef& window = ci::app::getWindow()) {

		fonte = Font( loadAsset("SimplonBP-Regular.otf"), 50.0 );
		mFont = gl::TextureFont::create( Font( loadAsset("SimplonBP-Medium.otf"), 20 ));
		//, gl::TextureFont::Format().enableMipmapping()

		static vector<signals::Connection> sWindowConnections;
		sWindowConnections = {
			window->getSignalMouseDown().connect( mouseDown ),
			window->getSignalMouseDrag().connect( mouseDrag ),
			//window->getSignalDraw().connect ( draw ),
			//			window->getSignalMouseUp().connect( mouseUp ),
			//			window->getSignalMouseMove().connect( mouseMove ),
			//			window->getSignalMouseWheel().connect( mouseWheel ),
			window->getSignalKeyDown().connect( keyDown ),
			window->getSignalKeyUp().connect( keyUp ),
			//			window->getSignalResize().connect( resize ),
		};

		mFbo = gl::Fbo::create( 320,800 );

		cores.push_back(Colorf(160/255.0f,  43/255.0f, 255/255.0f));  // 0 roxo
		cores.push_back(Colorf(234/255.0f,  40/255.0f, 140/255.0f));  // 1 rosa
		cores.push_back(Colorf(255/255.0f,  109/255.0f, 0/255.0f));   // 2 laranja
		cores.push_back(Colorf(255/255.0f,  255/255.0f, 0/255.0f));   // 3 amarelo
		cores.push_back(Colorf(0/255.0f,  211/255.0f, 24/255.0f));    // 4 verde
		cores.push_back(Colorf(0/255.0f,  206/255.0f, 255/255.0f));   // 5 azul

		coresOver.push_back(Colorf(122/255.0f,  35/255.0f, 204/255.0f));  // 0 roxo OK
		coresOver.push_back(Colorf(234/255.0f,  40/255.0f, 140/255.0f));  // 1 rosa
		coresOver.push_back(Colorf(255/255.0f,  109/255.0f, 0/255.0f));   // 2 laranja
		coresOver.push_back(Colorf(255/255.0f,  255/255.0f, 0/255.0f));   // 3 amarelo
		coresOver.push_back(Colorf(0/255.0f,  211/255.0f, 24/255.0f));    // 4 verde
		coresOver.push_back(Colorf(0/255.0f,  175/255.0f, 204/255.0f));   // 5 azul OK

	}


	void draw() {
		//cout << "draw ui " << endl;
		if (redraw) {
			//mFbo->bindFramebuffer();
			if (useFbo) {
				gl::ScopedFramebuffer fbScp( mFbo );
				gl::clear(ColorA(0,0,0,0));
				gl::enableAlphaBlending();
				for (auto & e : elements) {
					e.draw();
				}
			}

			else {
				for (auto & e : elements) {
					e.draw();
				}
			}
		}

		if (useFbo)
		{
			gl::color(1,1,1);
			gl::draw( mFbo->getColorTexture(), Area (0,0,320,800), Rectf(0,getWindowHeight()-800,320,800) ); //, Rectf(0,800,320,0)
			redraw = false;
		}
	}

	void createBool(string nome) {
		element te;
		te.tipo = LABEL;
		te.nome = nome;
		te.rect = Rectf(flow.x, flow.y, flow.x + sliderWidth, flow.y + sliderHeight);
		elements.push_back(te);
		flow.y += sliderHeight + sliderMargin;
	}

	void createLabel(string nome) {
		element te;
		te.tipo = LABEL;
		te.nome = nome;
		te.rect = Rectf(flow.x, flow.y, flow.x + sliderWidth, flow.y + sliderHeight);
		elements.push_back(te);
		flow.y += sliderHeight + sliderMargin;
	}

	void createSlider(string nome, float min, float max, float def, elementType tipo = SLIDER) {
		//int hue = int(flow.x + flow.y/6.0)%255;
		element te; // temp element
//		te.tipo = tipo;
//		te.nome = nome;
		te.rect = Rectf(flow.x, flow.y, flow.x + sliderWidth, flow.y + sliderHeight);
		te.min = min;
		te.max = max;
		te.def = def;
		te.init(nome, tipo);

		elements.push_back(te);
		flow.y += sliderHeight + sliderMargin;

//		cout << "create slider: " + nome << endl;
//		cout << min << endl;
//		cout << max << endl;
//		cout << def << endl;
//		cout << tipo << endl;
//		cout << "----" << endl;
	}


	void loadSliders(string arq) {
		std::string line;
		fs::path arquivo = getAssetPath("") / arq;
		ifstream myfile(arquivo.string() );
		//cout << arquivo.string() << endl;

		vector<string> myLines;
		while (std::getline(myfile, line))
		{
			myLines.push_back(line);
			vector<string> tabs = split( line, "\t" );

			//cout << line << endl;
			if (line == "") {
				//uis[ui]->addLabel("");
			} else {
				string tipo = tabs[0];
				string nome = tabs[1];
				if (tipo == "label") {
					//uis[ui]->addLabel(nome);
					createLabel(nome);
				}
				else if (tipo == "largelabel") {
					//uis[ui]->addLabel(nome, FontSize::LARGE);
				}
				if (tipo == "squeezy") {

				}
				else if (tipo == "float" || tipo == "slider" || tipo == "int") {
					vector<string> valores = split(tabs[2]," ");
					float min = stof(valores[0]);
					float max = stof(valores[1]);
					float def = stof(valores[2]);
					elementType t = SLIDER;
					if (tipo == "int") {
						t = SLIDERINT;
						pInt[nome] = def;
					} else {
						pFloat[nome] = def;
					}
					createSlider(nome, min, max, def, t);

				} else if (tipo == "toggle" || tipo == "bool") {
					pBool[nome] = stoi(tabs[2]);
					createBool(nome);
					//uis[ui]->addToggle(nome, &pBool[nome]);
				}
			}
		}
	}



}; // end namespace;