/*


 CinderImGui.cpp
 // wrong... and would not work in a multi-windows scenario
 na verdade ele serve pra fazer o bind nas conexoes. ta beleza.
 static vector<signals::Connection> sWindowConnections;
 sWindowConnections.push_back( window->getSignalDraw().connect( newFrameGuard ) );

 make use of events. signals?
 make two kinds of rects. one for layout and another for clicking
 useful in toggle buttons for example

 */


#include "cinder/Xml.h"
#include "cinder/gl/TextureFont.h"

namespace DmtrUI {

	using namespace ci;
	using namespace ci::app;
	using namespace std;
	using namespace cinder::gl;

	bool useCustomFont = false;
	// teste. nao sei se vai funcionar

	static vector<signals::Connection> signals;

	gl::FboRef			mFbo;
	Rectf coluna;
	bool redraw = true;
	bool useFbo = false;

	string presetsFolder = "presets/";

	map <string, float> 		pFloat;
	map <string, float> 		pEasy;
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
	float easing = 4.0;

	enum elementType {
		SLIDER, SLIDERINT, TOGGLE, LABEL, RADIO, RADIOITEM
	};

	class element {
	public:
		elementType tipo;
		float 	*_val;
		int   	*_valInt;
		bool	  	*_valBool;
		string	*_valString;
		// 'auto' not allowed in non-static class member
		//auto		*_autoVal;
		string 	nome;
		Rectf 	rect;
		Color	cor;
		ColorA 	corBg = ColorA(0,0,0,.4);
		ColorA 	textColor;
		float	min = 0;
		float	max = 1;
		float	def = .5;
		vec2		textOffset;

		// radio, que tal? recursion. will it work? YESS
		vector <element> elements;

		bool		selected = false;

		// seria lindo mas traz "auto return without trailing return type"
//		auto getVal() -> decltype(*_autoVal) {
//			if (tipo == SLIDER) {
//				return *_val;
//			}
//			else if (tipo == SLIDERINT) {
//				return *_valInt;
//			}
//		}

		// meant to load radio button from xml file and set children states.
		void updateRadio() {
			for (auto & e : elements) {
				e.selected = e.nome == *_valString;
			}
			redraw = true;
		}

		void init(string n, elementType t) {
			nome = n;
			tipo = t;
			cor = cores[0];
			corBg = coresOver[0];

			// local
			int textY = 18;

			if (tipo == SLIDER) {
				_val = &pFloat[nome];
                *_val = max * (def / max);
			}
			else if (tipo == SLIDERINT) {
				_valInt = &pInt[nome];
                *_valInt = max * (def / max);
				cor = cores[5];
				corBg = coresOver[5];
			}
			else if (tipo == TOGGLE) {
				cor = cores[4];
				corBg = coresOver[4];

				textOffset = vec2(sliderHeight + sliderMargin, textY);
				_valBool = &pBool[nome];
			}
			// NOT YET
			else if (tipo == RADIO) {
				_valString = &pString[nome];
			}

			else if (tipo == LABEL) {
				textOffset = vec2(0, textY);
			}

			if (tipo == LABEL || tipo == TOGGLE) {
				textColor = ColorA(0,0,0,1);
			}

			if (tipo == SLIDER || tipo == SLIDERINT) {
				textOffset = vec2(8, textY);
				textColor = ColorA(1,1,1,1);
			}

			if (tipo == RADIO) {

			}

			if (tipo == RADIOITEM) {
				textColor = ColorA(1,1,1,1);
				// nao sei se vai funcionar
				vec2 wh;
				if (useCustomFont) {
					wh = mFont->measureString(nome);
				} else {
					//wh = gl::measureString(nome);
				}
				int margemX = 6;
				rect.x2 = rect.x1 + wh.x + margemX*2;
				rect.y2 = rect.y1 + wh.y + 4;
				textOffset = vec2(margemX, textY);
			}
		}

		vec2 getDimensions() {
			return vec2(rect.x2-rect.x1, rect.y2-rect.y1);
		}

		void draw() {
			string texto = nome;
			if (tipo == SLIDER || tipo == SLIDERINT || tipo == TOGGLE) {
				color(cor);
				drawSolidRect(rect);
				float x2;
				if (tipo == SLIDER) {
					x2 = lmap<float>(*_val, min, max, 0, rect.x2 - rect.x1);
					texto += " : " + to_string(*_val);
				}
				else if (tipo == SLIDERINT) {
					x2 = lmap<int>(*_valInt, min, max, 0, rect.x2 - rect.x1);
					texto += " : " + to_string(*_valInt);

				}

				if (tipo == TOGGLE) {
					if (*_valBool) {
						color(corBg);
						int margem = 5;
						drawSolidRect(Rectf(rect.x1 + margem , rect.y1 + margem, rect.x2 - margem, rect.y2 - margem));
					}
				} else {
					color(corBg);
					drawSolidRect(Rectf(rect.x1, rect.y1, rect.x1 + x2, rect.y2));
				}
			}

			if (tipo == RADIOITEM) {
				color(cor);
				drawSolidRect(rect);
				if (selected) {
					color(corBg);
					drawSolidRect(rect);
				}
			}

			if (tipo == RADIO) {
				//cout << "radio draw" << endl;
				gl::pushMatrices();
				gl::translate(rect.x1, rect.y1);
				//cout << rect << endl;
				for (auto & e : elements) {
					e.draw();
					//cout << e.nome << endl;
				}
				gl::popMatrices();
			}
			color(textColor);
			//if (mFont->)
			if (useCustomFont) {
				mFont->drawString(texto, rect.getUpperLeft() + textOffset);
			} else {
				gl::drawString(texto, rect.getUpperLeft() + textOffset);
			}
		}

		void checkMouse(vec2 mouse) {
			if (tipo == SLIDER) {
				*_val = lmap<float>(mouse.x , rect.getX1(), rect.getX2(), min, max);
			}
			else if (tipo == SLIDERINT) {
				*_valInt = lmap<int>(mouse.x , rect.getX1(), rect.getX2(), min, max);
			}
			else if (tipo == TOGGLE) {
				*_valBool = !*_valBool;
				//cout << *_valBool << endl;
			}
			else if (tipo == RADIO) {
				for (auto & e : elements) {
					vec2 mouseOffset = vec2(mouse.x - rect.x1, mouse.y - rect.y1);
					if (e.rect.contains(mouseOffset)) {
						e.checkMouse(mouseOffset);
						*_valString = e.nome;
						e.selected = true;
					} else {
						e.selected = false;
					}
					// checar aqui se é exclusivo ou não. comparar com o valor anterior e disparar evento?
					//
				}
			}
			else if (tipo == RADIOITEM) {
				// NOT NEEDED
				//cout << "checkmouse radioitem" << endl;
				//selected = true;
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
		XmlTree saveUI ("DmtrUI", "");
		for (auto & e : elements) {
			if (e.tipo == SLIDER) {
				saveUI.push_back( XmlTree(e.nome, to_string(*e._val)));
			}
			else if (e.tipo == SLIDERINT) {
				saveUI.push_back( XmlTree(e.nome, to_string(*e._valInt)));
			}
			else if (e.tipo == TOGGLE) {
				saveUI.push_back( XmlTree(e.nome, to_string(*e._valBool)));
			}
			else if (e.tipo == RADIO) {
				saveUI.push_back( XmlTree(e.nome, (*e._valString)));
			}

		}
		saveUI.write( writeFile(filename) );
	}

	void load(string filename) {
		if (fs::exists(filename)) {
			XmlTree doc ( loadFile (filename) );
			XmlTree ui =  doc.getChild("DmtrUI");
			for (auto & e : elements) {
				if (e.tipo == SLIDER) {
						// try , catch ?
					if (ui.hasChild(e.nome)) {
						*e._val = (ui.getChild(e.nome).getValue<float>());
					}
				}
				else if (e.tipo == SLIDERINT) {
					if (ui.hasChild(e.nome)) {
						*e._valInt = (ui.getChild(e.nome).getValue<int>());
					}
				}
				else if (e.tipo == TOGGLE) {
					if (ui.hasChild(e.nome)) {
						*e._valBool = (ui.getChild(e.nome).getValue<bool>());
					}
				}
				else if (e.tipo == RADIO) {
					if (ui.hasChild(e.nome)) {
						*e._valString = (ui.getChild(e.nome).getValue<string>());
					}
					e.updateRadio();
				}
			}
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
			string nome = presetsFolder + string(1,key) + ".xml";
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

	void update() {
		easing = pFloat["easing"];
		for (auto & p : pFloat) {
			if (easing > 0) {
				pEasy[p.first] += (pFloat[p.first] - pEasy[p.first])/easing;
			}
			else {
				pEasy[p.first] = pFloat[p.first];
			}
		}
	}


	void draw() {
		if (redraw) {
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
			gl::draw( mFbo->getColorTexture(), Area (0,0,320,800), Rectf(0,getWindowHeight()-800,320,800) );
			redraw = false;
		}
	}

	void loadFont (string fontFile) {
		// Fonte rendering nao muito bom ainda. como melhorar? seria algo do retina?
		//fonte = Font( loadAsset("SimplonBP-Regular.otf"), 50.0 );
		//		string fontFile = "SimplonBP-Medium.otf";
		//string fontFile = "SimplonBP-Regular.otf";
		mFont = gl::TextureFont::create( Font( loadAsset(fontFile), 18 ));
		//, gl::TextureFont::Format().enableMipmapping()
		useCustomFont = true;
	}


	void setup( const ci::app::WindowRef& window = ci::app::getWindow()) {
		static vector<signals::Connection> sWindowConnections;
		sWindowConnections = {
			window->getSignalMouseDown().connect( mouseDown ),
			window->getSignalMouseDrag().connect( mouseDrag ),
			window->getSignalDraw().connect( update ),
			window->getSignalPostDraw().connect ( draw ),
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
		coresOver.push_back(Colorf(0/255.0f,  158/255.0f, 11/255.0f));    // 4 verde OK
		coresOver.push_back(Colorf(0/255.0f,  175/255.0f, 204/255.0f));   // 5 azul OK
	}




	// CREATE ELEMENTS
	void createRadio(string nome, vector <string> options) {
		element te;
		vec2 flowing = vec2(0,0);
		for (auto & o : options) {
			element tc; // temporary child
			tc.nome = o;
			int w = 120;
			int h = sliderHeight;
			tc.rect = Rectf(flowing.x, flowing.y, flowing.x + w, flowing.y + h);
			tc.init(o, RADIOITEM);

			te.elements.push_back(tc);
			vec2 dimensoes = tc.getDimensions();
			flowing.x += dimensoes.x + 2;
		}
		te.rect = Rectf(flow.x, flow.y, flow.x + sliderWidth, flow.y + sliderHeight);
		te.init(nome, RADIO);
		elements.push_back(te);
		flow.y += sliderHeight + sliderMargin;
	}

	void createBool(string nome) {
		element te;
		te.rect = Rectf(flow.x, flow.y, flow.x + sliderHeight, flow.y + sliderHeight);
		te.cor = Color(1,0,.3);
		te.init(nome, TOGGLE);
		elements.push_back(te);
		flow.y += sliderHeight + sliderMargin;
	}

	void createLabel(string nome) {
		element te;
		te.rect = Rectf(flow.x, flow.y, flow.x + sliderWidth, flow.y + sliderHeight);
		te.init(nome, LABEL);
		elements.push_back(te);
		flow.y += sliderHeight + sliderMargin;
	}

	void createSlider(string nome, float min, float max, float def, elementType tipo = SLIDER) {
		//int hue = int(flow.x + flow.y/6.0)%255;
		element te;
		te.rect = Rectf(flow.x, flow.y, flow.x + sliderWidth, flow.y + sliderHeight);
		te.min = min;
		te.max = max;
		te.def = def;
		te.init(nome, tipo);
		elements.push_back(te);
		flow.y += sliderHeight + sliderMargin;
	}

	void loadSliders(string arq) {
		std::string line;
		fs::path arquivo = getAssetPath("") / arq;
		ifstream myfile(arquivo.string() );
		vector<string> myLines;
		while (std::getline(myfile, line)) {
			myLines.push_back(line);
			vector<string> tabs = split( line, "\t" );
			if (line == "") {
				createLabel("");
			} else {
				string tipo = tabs[0];
				string nome = tabs[1];
				if (tipo == "label") {
					createLabel(nome);
				}
				else if (tipo == "largelabel") {
					// large
					createLabel(nome);
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
				}
				else if (tipo == "radio") {
					createRadio(nome, split(tabs[2], " "));
				}
			}
		}
	}

}; // end namespace;