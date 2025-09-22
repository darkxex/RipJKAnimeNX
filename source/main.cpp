#include <string>
#include "JKanime.hpp"
#include "extra.hpp"
#include "input.hpp"
//make some includes to clean a little the main
string urlc = "https://myrincon.es";

//MAIN INT
int main(int argc, char **argv)
{
    //Algunos servicios hay que inicializarlos
	socketInitializeDefault();
	romfsInit();
	nxlinkStdio();
	printf("Nxlink server Conected\n");

	//Montar Partición USER como RW
	emmc::init();

	LOG::init();//control de logs
	user::initUser();//obtener usuarios de la consola
	AppletMode=GetAppletMode();//verificar si estamos en el Álbum o a full memoria
	ChainManager(true,true);//Esto deshabilita el apagado de la pantalla y aumenta ligeramente el reloj de la cpu
	isConnected=Net::HasConnection();//verifica si hay conexión a internet

    //Intentamos usar el fichero de temas en themas:/ como RO
	mount_theme("themes",true);

    //Intentamos montar la salva de Usuario en save:/ como RW
	user::MountUserSave();

	/* Leer las bases de datos.
    AB = AnimeMeta.json,
        Guarda los datos de los animes solamente
    BD = DataBase.json,
       Guarda las listas construidas de animes que van saliendo
        y otros ajustes generales como: ultimo anime, búsquedas, etc
        con el fin de pre cargar esto al inicio y no tener q esperar
    UD = UserData.json
        Guarda información mas personal para cada usuario como historial,favoritos 
        y otra configuración solo de ese usuario
    */
	if(!read_DB(AB,rootdirectory+"AnimeMeta.json")){
		read_DB(AB,"romfs:/AnimeMeta.json");//usar solo si no existe ninguna base de datos en memoria
	}
	read_DB(BD,rootdirectory+"DataBase.json");

    //Estos son algunos fix que se han hecho a lo largo de las versiones, los cambios y errores detectados 
    if (BD["arrays"]["chapter"]["link"] == "[null]"_json ){BD["arrays"]["chapter"]["link"]="null"_json;}
	BD["com"] = "{}"_json;
	if (isset(BD["arrays"],"Banner")) {
		BD["arrays"].erase("Banner");
	}
	if (isset(BD,"DataBase")) {
		BD.erase("DataBase");
	}
	if (isset(BD,"USER")) {
		UD = BD["USER"];
		BD.erase("USER");
	}

	read_DB(UD,rootsave+"UserData.json");

	GOD.intA();//Iniciar el motor SDL para dibujar gráficos
    
    //Crear carpetas si no existen
    if(!isFileExist(rootdirectory+"DATA")){
		mkdir(rootdirectory.c_str(), 0777);
		mkdir((rootdirectory+"DATA").c_str(), 0777);
    }
	mkdir((rootdirectory+"TEMP").c_str(), 0777);

    //delete old base
    if(isFileExist(rootdirectory+"AnimeBase.json")){
        remove((rootdirectory+"AnimeBase.json").c_str());
    }

    GOD.SkinInit(roottheme);//Esto carga la lista de skins
    GOD.SkinInit(oldroot+"theme/",true);//Esto añade a la lista de skins
	GOD.loadSkin();//Esto carga la skin guardada y la música

    //Estas son algunas variables que serán útiles luego
	gTextTexture.mark=false;//deshabilitar táctil para este objeto
	Farest.mark=false;//deshabilitar táctil para este objeto
	USER.loadFromFileCustom(rootsave+"User.jpg",58, 58);

	SDL_Color textColor = { 50, 50, 50 };
	SDL_Color textWhite = { 255, 255, 255 };
	SDL_Color textBlue = { 100, 100, 255 };
	SDL_Color textGray = { 130, 130, 130 };
	SDL_Color textGrayGreen = { 90, 170, 90 };
	SDL_Color textWhiteGreen = { 80, 255, 80 };

	int posxbase = 20;
	int posybase = 10;

    //Inicializar el controlador de interfaz táctil y de controles
	Inputinit();
    std::thread CapS;
    std::thread CapP;
    bool CapSC = false;
    bool CapPC = false;
        

    //parte principal protegida por un try para obtener fallos si ocurren
	try{
        Net::init();//Establecer init de red
		//Cargar imágenes de uso general guardadas en la romfs 
		LoadImages();
        
        //revisemos si estamos en sxos
		if (isSXOS) {
			GOD.GenState = statenow;
			GOD.JKMainLoop();
			Farest.render((0), (0));
			GOD.Confirm("Esta App No funciona Correctamente En SXOS",true,3);
		}

        //Este Thread se encarga de cargar casi todo en segundo plano imagenes, datos, Animes, temas, etc ver JKanime.cpp
		Loaderthread = SDL_CreateThread(AnimeLoader, "Loaderthread", (void*)NULL);
		
        //este se encarga de los inputs
		inputThread = SDL_CreateThread(inputAsync, "inputThread", (void*)NULL);
        
        //Esto se usa para evitar el cierre forzado de la app, poder capturar dicho evento y cerrar con normalidad.
		//if (!AppletMode)
            appletLockExit();

		//Bucle principal mientas la app corre, salirse de aquí significa cerrarla, (se saldrá si se fuerza el cierre)
		while (GOD.JKMainLoop())
		{
			//Limpiar pantalla
			SDL_SetRenderDrawColor(GOD.gRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(GOD.gRenderer);

			//Dibujar Fondo de pantalla asignado
			Farest.render((0), (0));

			//dibujar caja de abajo donde van los botones
			VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);

			//Renderizar las fases del interfaz de Usuario
			switch (statenow)
			{
            //sección de apertura de anime
			case chapter_s: {
				//Dibujar recuadros donde posar elementos
				USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);
				VOX.render_VOX({0,0, SCREEN_WIDTH, 670},170, 170, 170, 100);
				VOX.render_VOX({0,0, 1280, 60},200, 200, 200, 130);

                //Datos iniciales de anime
				GOD.HR=200; GOD.HG=200; GOD.HB=200;
				string temptext = BD["com"]["ActualLink"];
				NameOfLink(temptext);


				//Dibujar Titulo
				gTextTexture.loadFromRenderedText(GOD.Arista_40, temptext.substr(0,62)+ ":", textColor);
				gTextTexture.render(posxbase, posybase);

				{//Dibujar imagen de anime
					VOX.render_VOX({ SCREEN_WIDTH - 412,63, 404, 590}, 0, 0, 0, 200);
					GOD.Image(BD["com"]["ActualLink"],SCREEN_WIDTH - 410, 65,400, 550,BT_B);
				}

				{//Dibujar descripcion
					int XG=10,YG=63,WG=850,HG=50;
					static string rese_prot = "..";
					string rese_p = BD["com"]["sinopsis"];
                    rese_p = rese_p.substr(0,1200);
					if (rese_prot != rese_p) {//recargar textura solo si el texto cambia (no lo hace, va mas rápido asi)
						T_R.loadFromRenderedTextWrap(GOD.Comic_16, rese_p, textColor, WG-20);
						cout << "desc:" << T_R.getHeight()+60 << endl;
                        rese_prot = rese_p;
					}
					HG = T_R.getHeight()+60;
					HG = HG < 340 ? 340 : HG;

					VOX.render_VOX({XG, YG, WG, HG}, 255, 255, 255, 170);
					T_R.render(XG+10, YG+10);

					gTextTexture.loadFromRenderedTextWrap(GOD.Comic_16, BD["com"]["Emitido"], textColor,WG-20);
					gTextTexture.render(XG+10, YG+HG-(gTextTexture.getHeight()*2)-5);

					gTextTexture.loadFromRenderedTextWrap(GOD.Comic_16, BD["com"]["generos"], textColor,WG-20);
					gTextTexture.render(XG+10, YG+HG-gTextTexture.getHeight()-5);
				}

				bool anend=false;
				int sizefix = 0;
				if (maxcapit >= 0) {
					if (BD["com"]["nextdate"] == "Pelicula") {
						gTextTexture.loadFromRenderedText(GOD.Arista_40, "Pelicula", { 250,250,250 });
						gTextTexture.render(posxbase + 855, posybase + 598);
					} else {
						if (BD["com"]["enemision"] == "true")
						{
							if(BD["com"]["estado"].is_null()) {
								BD["com"]["estado"] = "En Emisión";
							}
							gTextTexture.loadFromRenderedText(GOD.Arista_40, BD["com"]["estado"], { 16,191,0 });
							gTextTexture.render(posxbase + 855, posybase + 598);
						} else {
							gTextTexture.loadFromRenderedText(GOD.Arista_40, "Concluido", { 140,0,0 });
							gTextTexture.render(posxbase + 855, posybase + 598);
						}
						gTextTexture.loadFromRenderedText(GOD.Comic_16, BD["com"]["nextdate"], { 255,255,255 });
						gTextTexture.render(posxbase + 1010, posybase + 615);
					}
				}

				{//Control deslizante de capítulos
					int XS=240, YS =0;//use esto para mover el lemento entero
					if (maxcapit >= 0) {
						int mwide = 35;//52
						int XD = 210+XS, YD = 582+YS;
						sizefix = (int)arrayservers.size() * mwide;
						anend=VOX.render_AH(XD, YD, 190, sizefix, serverpront);
						if(serverpront) {
							if (anend) {
								for (int x = 0; x < (int)arrayservers.size(); x++) {
									if (x == selectserver) {
										T_T.loadFromRenderedText(GOD.Arista_30, arrayservers[x], textWhite);
										VOX.render_VOX({ posxbase+XD-10,YD + 5 - sizefix + (x * mwide), 170, T_T.getHeight()-5}, 50, 50, 50, 200);
										T_T.render(posxbase+XD, YD - sizefix + (x * mwide));
									} else {
										gTextTexture.loadFromRenderedText(GOD.Arista_30, arrayservers[x],textGray);
										gTextTexture.render(posxbase+XD, YD - sizefix + (x * mwide));
									}
								}
							}
						}
					}
					if(serverpront) {
						if (anend) {
							B_UP.render_T(280+XS, 530+YS-sizefix,"");
						}
						B_DOWN.render_T(280+XS, 630+YS,"");
					}
					if (maxcapit >= 0&&BD["com"]["nextdate"] != "Pelicula") {//draw caps numbers Slider
						VOX.render_VOX({posxbase + 50+XS, posybase + 571+YS, 460, 33 }, 50, 50, 50, 200);
						SDL_Color com = {};
						if (latest-2 >= mincapit) {
							com=textGray;
							if (latest-2 == latestcolor) com=textGrayGreen;
							gTextTexture.loadFromRenderedText(GOD.AF_35,  to_string(latest-2), com);
							gTextTexture.render(posxbase + 120 +XS-gTextTexture.getWidth()/2, posybase + 565+YS);
						}
						if (latest-1 >= mincapit) {
							com=textGray;
							if (latest-1 == latestcolor) com=textGrayGreen;
							gTextTexture.loadFromRenderedText(GOD.AF_35,  to_string(latest-1), com);
							gTextTexture.render(posxbase + 200+XS-gTextTexture.getWidth()/2, posybase + 565+YS);
						}
						{
							com=textBlue;
							if (latest == latestcolor) com=textWhiteGreen;
							if (serverpront) {
								T_N.loadFromRenderedText(GOD.AF_35, to_string(latest), com);
								T_N.render(posxbase + 280+XS-T_N.getWidth()/2, posybase + 565+YS);
							} else {
								T_T.loadFromRenderedText(GOD.AF_35, to_string(latest), com);
								T_T.render(posxbase + 280+XS-T_T.getWidth()/2, posybase + 565+YS);
							}
						}

						if (latest+1 <= maxcapit) {
							com=textGray;
							if (latest+1 == latestcolor) com=textGrayGreen;
							gTextTexture.loadFromRenderedText(GOD.AF_35,  to_string(latest+1), com);
							gTextTexture.render(posxbase + 360+XS-gTextTexture.getWidth()/2, posybase + 565+YS);
						}
						if (latest+2 <= maxcapit) {
							com=textGray;
							if (latest+2 == latestcolor) com=textGrayGreen;
							gTextTexture.loadFromRenderedText(GOD.AF_35,  to_string(latest+2), com);
							gTextTexture.render(posxbase + 440+XS-gTextTexture.getWidth()/2, posybase + 565+YS);
						}

						if (maxcapit >= 10 && !serverpront) {
							B_UP.render_T(280+XS, 530+YS,"+10",serverpront);
							B_DOWN.render_T(280+XS, 630+YS,"-10",serverpront);
						}

						B_LEFT.render_T(45+XS, 580+YS,to_string(mincapit),latest == mincapit);
						B_RIGHT.render_T(515+XS, 580+YS,to_string(maxcapit),latest == maxcapit);
					} else {
						VOX.render_VOX({posxbase + 185+XS, posybase + 570+YS, 200, 35 }, 50, 50, 50, 200);
						if (BD["com"]["nextdate"] == "Pelicula" || mincapit == maxcapit) {
							T_T.loadFromRenderedText(GOD.Arista_40, "Reproducir...", { 255, 255, 255 });
							T_T.render(posxbase + 282+XS-T_T.getWidth()/2, posybase + 558+YS);
						} else {
							gTextTexture.loadFromRenderedText(GOD.Arista_40, "Cargando...", { 255, 255, 255 });
							gTextTexture.render(posxbase + 282+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
						}

					}
				}

				//Dibujar botones
				int dist = 1095,posdist = 160;
				if(serverpront) {
					B_A.render_T(dist, 680,"Ver Online"); dist -= posdist;
					B_B.render_T(dist, 680,"Cerrar"); dist -= posdist;
				} else {
					B_A.render_T(dist, 680,"Aceptar"); dist -= posdist;
					B_B.render_T(dist, 680,"Atrás"); dist -= posdist;
					B_X.render_T(dist, 680,"Descargar"); dist -= posdist;
				}
                //comprobar si es favorito
				if(gFAV) {
					FAV.render(1225, 70);
				} else {
					NFAV.render_T(1225, 70,"");
					B_Y.render_T(dist, 680,"Favorito"); dist -= posdist;
				}

                //Dibujar Precuela y secuela si existe resolver congelamiento
				if (isset(AB[KeyName],"Secuela")) {
					string imagelocal=AB[KeyName]["Secuela"];
					imagelocal = KeyOfLink(imagelocal);
					imagelocal = rootdirectory+"DATA/"+imagelocal+".jpg";
                    if(!serverpront) {B_R.render_T(dist, 680,"Secuela"); dist -= posdist;}
                    if (!isFileExist(imagelocal)){
                        if (!CapSC){
                            CapS = std::thread(CheckImgNet,imagelocal,"");
                            CapSC = true;
                        }
                    } else {
                        if (CapS.joinable()) {CapS.join();}
                        CapSC = false;
                        GOD.Cover(imagelocal,160,456,"Secuela",120,BT_R);
                    }
				}
				if (isset(AB[KeyName],"Precuela")) {
					string imagelocal=AB[KeyName]["Precuela"];
					imagelocal = KeyOfLink(imagelocal);
					imagelocal = rootdirectory+"DATA/"+imagelocal+".jpg";
                    if(!serverpront) {B_L.render_T(dist, 680,"Precuela"); dist -= posdist;}
                    if (!isFileExist(imagelocal)){
                        if (!CapPC){
                            CapP = std::thread(CheckImgNet,imagelocal,"");
                            CapPC = true;
                        }
                    } else {
                        if (CapP.joinable()) {CapP.join();}
                        CapPC = false;
                        GOD.Cover(imagelocal,10,456,"Precuela",120,BT_L);
                    }
				}
				break;
			}
			case menu_s:
			case programation_s: {//este sera el manu principal donde salen los últimos capítulos emitidos 
				if (!reloading&&BD["arrays"]["chapter"]["link"].size()>=1) {

					if(ongrid) {// ongrid esta deprecad, cambia la vista de cuadricula a lista antigua.
						USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);
                        //definir comandos táctiles solo para esta instancia que abran el menú con un deslizado
						if (GOD.fingermotion_LEFT & (GOD.TouchX > 1100)) {
							GOD.fingermotion_LEFT = false;
							statenow=menu_s;
						}
						if (GOD.fingermotion_RIGHT & (GOD.TouchX > 850)) {
							GOD.fingermotion_RIGHT = false;
							statenow=programation_s;
						}
						GOD.HR=200; GOD.HG=200; GOD.HB=200;//color de barra superior
						if (preview)//preview define si se ha terminado de cargar todo suele usarse solo en el primer arranque
						{
							GOD.ListCover(selectchapter,BD["arrays"]["chapter"],ongrid,Frames);
							if (isHandheld && statenow!=menu_s) {//iconos superiores visibles solo cuando esta en modo portatil
								FAVB.render(SCREEN_WIDTH - USER.getWidth() - FAVB.getWidth() - 20, 1);
								HISB.render(SCREEN_WIDTH - USER.getWidth() - FAVB.getWidth() - BUS.getWidth() - 50, 1);
								BUSB.render(SCREEN_WIDTH - USER.getWidth() - FAVB.getWidth() - BUS.getWidth() - HISB.getWidth() - 70, 1);
							}
						} else {
							VOX.render_VOX({ 0,14, 50, 46}, 255, 255, 255, 200);
						}
                        
                    
                        //Animacion de rotacion un lado a otro
                        //Al rotar indica trabajo en segundo plano
						double angle = 0.0;
						static int Ticker=0;
						if (isChained) {
							REC.TickerRotate(Ticker,0,360,2,isChained);
							angle=Ticker;
						} else if (Ticker > 0) {
							REC.TickerRotate(Ticker,-25,360,5,isChained);
							angle=Ticker;
						}else if (Ticker < 0) {
							Ticker=0;
						}

						if (part > 0) {
                            gTextTexture.loadFromRenderedText(GOD.digi_9, to_string(ofall - part), {50,50,50});
                            gTextTexture.render(27 - (gTextTexture.getWidth()/2), 30);
						}
                        //if ( part % 2 == 0){
                        if (Ticker > 0){
                            if ((Ticker % 10) > 5){
                                Heart.render(19, 30);
                            }
                        }

						REC.render(5, 15,NULL,angle);
                        
					} else {//Lista Clasica
						GOD.ListClassic(selectchapter,BD["arrays"]["chapter"]);
						if (preview)
						{
							GOD.ListCover(selectchapter,BD["arrays"]["chapter"]);
						}
						B_UP.render_T(580, 5,"");
						B_DOWN.render_T(580, 630,"");
					}

					//Dibujar Cabecera y titulo del anime
					gTextTexture.loadFromRenderedText(GOD.digi_11, ("(Ver "+DInfo()["App"].get<string>()+") #KASTXUPALO").c_str(), {100,0,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 3, 672);

					gTextTexture.loadFromRenderedText(GOD.Comic_16, "Recientes", {100,0,0});
					if(ongrid) {
						gTextTexture.render(5, 1);
					}else {
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
						if (imgNumbuffer > 0) {
							gTextTexture.loadFromRenderedText(GOD.Comic_16, "Imágenes: ("+to_string(imgNumbuffer)+"/"+to_string(imgNumbufferAll)+")", {0,100,0});
							gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 22);
							//Heart.render(posxbase + 570, posybase + 3 + (imgNumbuffer-1) * 22);
						}
						if (part > 0) {
							gTextTexture.loadFromRenderedText(GOD.Comic_16, "Búfer: ("+to_string(part)+"/"+to_string(ofall)+")", {0,100,0});
							gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 22);
						}
					}
				} else {
                    //pantalla de carga, suele  aparecer solo en la primera apertura de la app
					string textpro="Cargando programación";
					if(imgNumbuffer>0) {textpro+=" "+to_string(imgNumbuffer)+"/"+to_string(imgNumbufferAll)+"";} else {textpro+="...";}
					GOD.PleaseWait(textpro,false);
				}
                
                //Menu lateral (Falta mejorarlo)
				if (statenow==menu_s) {

					{//Esto dibuja un menú lateral de opciones para moverse entre las diferentes instancias
						StatesList= {"Búsqueda","Historial","Favoritos","En Emisión","Top Anime","Nuevos","AnimeFLV"};
						if(isDownloading) {StatesList.push_back("Descargas");}

						int mwide = 60,XD=940,YD=120,W=1280-XD;
						VOX.render_VOX({XD,61, 1280, 608}, 160, 160, 160, 220);        //draw area
						VOX.render_VOX({XD,61, W, 1}, 255, 255, 255, 235);        //head line
						VOX.render_VOX({XD,668, W, 1}, 255, 255, 255, 235);        //bottom line
						VOX.render_VOX({XD,61, 1, 607}, 255, 255, 255, 235);        //line left
						gTextTexture.loadFromRenderedText(GOD.Arista_20, "Menú Primario",textColor);
						gTextTexture.render(XD+20, 65);
						if (imgNumbuffer > 0) {//esto muestra el progreso en segundo plano
							gTextTexture.loadFromRenderedText(GOD.Comic_16, "Imágenes: ("+to_string(imgNumbuffer)+"/"+to_string(imgNumbufferAll)+")", {0,100,0});
							gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 70);
						}
						if (part > 0) {
							gTextTexture.loadFromRenderedText(GOD.Comic_16, "Búfer: ("+to_string(part)+"/"+to_string(ofall)+")", {0,100,0});
							gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15,  70);
						}
						if (porcentajebufferF > 0) {
							gTextTexture.loadFromRenderedText(GOD.Comic_16, "BúferFav: ("+to_string(porcentajebufferF)+"/"+to_string(porcentajebufferFF)+")", {0,100,0});
							gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 70);
						}
						int indexLsize = StatesList.size();
						int Ymaxsize = indexLsize*mwide;
						if(GOD.TouchX < 1280 && GOD.TouchY < Ymaxsize+YD && GOD.TouchY > YD && GOD.TouchX > XD+100) {
							int sel=(GOD.TouchY-YD) / mwide;
							VOX.render_VOX({XD+80-10,YD+(sel*mwide)+5, W-100, mwide-5}, 0, 0, 255, 235);
							if (sel >= 0 && sel < indexLsize) {
								selectelement = sel;
							}
						}
                        //dibujar elementos del menú
						for (int x = 0; x < indexLsize; x++) {
							if(x == 0) {BUSB.render(XD+10, YD + (x * mwide)+5);}
							if(x == 1) {HISB.render(XD+10, YD + (x * mwide)+5);}
							if(x == 2) {FAVB.render(XD+10, YD + (x * mwide)+5);}
							if(x == 3) {HORB.render(XD+10, YD + (x * mwide)+5);}
							if(x == 4) {TOPB.render(XD+10, YD + (x * mwide)+5);}
							if(x == 5) {GOD.MapT["ULTB"].render(XD+10, YD + (x * mwide)+5);}
							if(x == 6) {AFLV.render(XD+10, YD + (x * mwide)+5);}
							if(x == 7) {DOWB.render(XD+10, YD + (x * mwide)+5);}
							if(x == 8) {DOWB.render(XD+10, YD + (x * mwide)+5);}

							if (x == selectelement) {
								T_T.loadFromRenderedText(GOD.digi_40, StatesList[x], textColor);
                                int cen = YD + (x * mwide) + ((mwide - T_T.getHeight() )/2);
								VOX.render_VOX({XD+80-3,cen + 5, W-100, T_T.getHeight()-5}, 50, 50, 50, 100);
								T_T.render(XD+85, cen);
							} else {
								gTextTexture.loadFromRenderedText(GOD.digi_40, StatesList[x],textWhite);
                                int cen = YD + (x * mwide) + ((mwide - gTextTexture.getHeight() )/2);
								gTextTexture.render(XD+85, cen);
							}

							if (x < indexLsize-1) {//separador
								VOX.render_VOX({XD+75,YD + (x * mwide)+mwide+2, W-105, 1}, 255, 255, 255, 235);
							}
						}
					}
                    
                    //Dibuja un banner con imagenes de animes recomendados
					int XF=10, YF=65, WF=760, HF=427;
                    if (BD["arrays"]["Benner"].size() > 0) {
                        VOX.render_VOX({XF-2, YF-2, WF+4, HF+4}, 255, 255, 255, 235);
						
						bannersize = BD["arrays"]["Benner"].size()-1;
						
						//Reloj 5s para pasar de una imagen a otra
						bool makebomb = false;
						if (inTimeN(5001,1)) {
							if (Btimer) {
								makebomb = true;
								bannersel++;
								if (bannersel>bannersize) bannersel=0;
							} else
								Btimer=true;
						}
						//string temptext = BD["arrays"]["Benner"]["link"][bannersel];
						//NameOfLink(temptext);
                        string path = rootdirectory+"DATA/nop.png";//ESTO puede ser un error
                        static string seudopath = "";
                        if (bannersize >= bannersel) {
                           path = BD["arrays"]["Benner"][bannersel]["file"];
                        }
                       string temptext = "NULL";
                       if (BD["arrays"]["Benner"][bannersel]["name"].is_string()){
                            temptext = BD["arrays"]["Benner"][bannersel]["name"];
                        }

						GOD.Image(path, XF, YF, WF, HF,BT_RIGHT);//Renderizar imagen principal

                        //salvar imagen y reiniciar la transparencia
                        static int alphaB = 255;
                        static string oldimage = "";
						if (makebomb) {
                            alphaB = 255;
                            oldimage = seudopath;
                            cout << "Alpha of : " << oldimage << " over : " << seudopath << endl;
						}
                        //Set seudo path
                        seudopath = path.substr(path.rfind("/")+1);

                        //replace(seudopath,rootdirectory+"TEMP/","");
                        //replace(seudopath,rootdirectory+"DATA/","");

                        //Efecto de disolver
                        if(oldimage != ""){
                            if (alphaB > 0){
                                if (inTimeN(51,0)) {
                                    alphaB-=30;
                                    if (alphaB < 100){alphaB-=10;}
                                    if (alphaB < 0){alphaB=0;}
                                    //cout << "Alpha of : " << oldimage << " set to:" << alphaB << " over : " << seudopath << endl;
                                }
                                GOD.MapT[oldimage].setAlpha(alphaB);
                                GOD.MapT[oldimage].render(XF, YF);
                           } 
                           
                           if (alphaB <= 0 || oldimage == seudopath){
                                GOD.MapT[oldimage].setAlpha(255);
                                oldimage = "";
                           }
                           
                        }
                        //This is a jump efect
                        /*
						if (makebomb) {
							GOD.MapT[seudopath].offboom_size=2;
							GOD.MapT[seudopath].TickerBomb();
							makebomb = false;
						}
                        
                        */
                        
                        //Titulo del anime en el banner
						VOX.render_VOX({XF, YF, WF, 30}, 255, 255, 255, 135);
						gTextTexture.loadFromRenderedText(GOD.Arista_30, temptext.substr(0,55)+ ":", textColor);
						gTextTexture.render(XF, YF-5);

						VOX.render_VOX({XF, YF+HF-45, 165, 45}, 255, 255, 255, 135);
						B_RIGHT.render_T(XF+5, YF+HF-40,"Ver Ahora");

                        //elemento temporal para el cambio de temas
						if (isset(UD["Themes"],"name")) {
							//Thema temporal
							static int WT = 190;
							VOX.render_VOX({XF, YF+HF+10, WT+15, 45}, 255, 255, 255, 185);
							WT = B_ZL.render_T(XF+5, YF+HF+15,"Tema: "+UD["Themes"]["name"].get<string>());
						}
					}

					//dibujar botones del fondo del menu
					int dist = 1100,posdist = 170;
					B_A.render_T(dist, 680,"Aceptar"); dist -= posdist;
					B_B.render_T(dist, 680,"Atrás"); dist -= posdist;
					if (isHandheld) {CLEAR.render_T(dist, 680,"Cache"); dist -= posdist;}
					break;
				}
				//dibujar botones del fondo de la pantalla principal
				int dist = 1100,posdist = 170;
				B_A.render_T(dist, 680,"Aceptar"); dist -= posdist;
				B_R.render_T(dist, 680,"Buscar"); dist -= posdist;
				//B_L.render_T(dist, 680,"AnimeFLV");dist -= posdist;
				B_P.render_T(dist, 680,"Menú"); dist -= posdist;
				if(isDownloading) {B_X.render_T(dist, 680,"Descargas"); dist -= posdist-10;}
                if(isHandheld) GOD.MapT["EXIT"].render_T(80, 680,"",quit);
				break;
			}
			case search_s:                    {
				if (!reloadingsearch) {
					int srchsize=BD["arrays"]["search"]["link"].size();
					if(ongrid) {USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);}
					if (srchsize > 0) {
						//if (srchsize > 30) ongrid=false;
						if (!ongrid) GOD.ListClassic(searchchapter,BD["arrays"]["search"]);
						if (preview)
						{
							if (ongrid) {
								VOX.TickerColor(GOD.HG,210,255, 200);
								GOD.HR=200; GOD.HB=200;
								GOD.ListCover(searchchapter,BD["arrays"]["search"],ongrid,Frames);
								BUS.render_T(5, 15,"");
								if (isHandheld) {
									BUSB.render(SCREEN_WIDTH - USER.getWidth() - FAVB.getWidth() - BUS.getWidth() - 50, 1);
								}
							} else {
								GOD.ListCover(searchchapter,BD["arrays"]["search"]);
								B_UP.render_T(580, 5,"");
								B_DOWN.render_T(580, 630,"");
							}
						}
					}else {
						NOP.render_T(230, 402,BD["searchtext"]);
					}
					//Draw Header
					gTextTexture.loadFromRenderedText(GOD.Comic_16, "Resultados de Búsqueda:", {100,0,0});
					if(ongrid) {
						int distan = gTextTexture.getWidth()+10;
						gTextTexture.render(5, 1);
						gTextTexture.loadFromRenderedText(GOD.Comic_16, BD["searchtext"], {0,0,0});
						VOX.render_VOX({distan-2,1, gTextTexture.getWidth()+4, gTextTexture.getHeight()}, 210, 210, 210, 155);
						//T_D.getWidth()+4, T_D.getHeight()
						gTextTexture.render(distan, 1);
					}else {
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
						gTextTexture.loadFromRenderedText(GOD.Comic_16, BD["searchtext"], {0,0,0});
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 22);
					}
					{//Draw footer buttons
						int dist = 1100,posdist = 160;
						B_A.render_T(dist, 680,"Aceptar"); dist -= posdist;
						B_B.render_T(dist, 680,"Atras"); dist -= posdist;
						B_R.render_T(dist, 680,"Buscar"); dist -= posdist;
					}
				}
				else
				{
					string added="";
					if(porcentajebufferF > 0) {
						int persen = ((porcentajebufferF) * 100) / porcentajebufferFF;
						added=" ("+to_string(persen)+"%)";
					}
					GOD.PleaseWait("Cargando búsqueda..."+added,false);
					Frames=1;
				}
				break;
			}
			case favorites_s:         {
				json VecF;
				VecF["link"]=UD["favoritos"];
				if(ongrid) {USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);}
				int favsize=UD["favoritos"].size();
				if (favsize > 0) {
					//if (favsize > 30) ongrid=false;
					if (!ongrid) GOD.ListClassic(favchapter,VecF);
					if (preview)
					{
						if (ongrid) {
							VOX.TickerColor(GOD.HR,210,255, 200);
							GOD.HG=200; GOD.HB=200;
							GOD.ListCover(favchapter,VecF,ongrid,Frames);
							FAV.render_T(5, 15,"");
						} else {
							GOD.ListCover(favchapter, VecF);
							B_UP.render_T(580, 5,"");
							B_DOWN.render_T(580, 630,"");
						}
					}
				}

				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.Comic_16, "Favoritos", {100,0,0});
				if(ongrid) {
					gTextTexture.render(5, 1);
				}else {
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);

					if (porcentajebufferF > 0) {
						gTextTexture.loadFromRenderedText(GOD.Comic_16, "Búfer: ("+to_string(porcentajebufferF)+"/"+to_string(porcentajebufferFF)+")", {0,100,0});
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);
					}
				}

				{//Draw footer buttons
					int dist = 1100,posdist = 180;
					B_A.render_T(dist, 680,"Aceptar"); dist -= posdist;
					B_B.render_T(dist, 680,"Volver"); dist -= posdist;
					if ((int)UD["favoritos"].size() >= 1) {
						B_X.render_T(dist, 680,"Borrar #"+to_string(favchapter+1)); dist -= posdist;
					}else NOP.render_T(230, 455,"");
				}
				break;
			}
			case history_s:           {
				if(ongrid) {USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);}
				json VecF;
				VecF["link"]=UD["history"];

				int histsize=VecF["link"].size();
				if (histsize > 0) {
					//if (favsize > 30) ongrid=false;
					if (!ongrid) GOD.ListClassic(histchapter,VecF);
					if (preview)
					{
						if (ongrid) {
							VOX.TickerColor(GOD.HG,100,200, 200);
							GOD.HR=200; GOD.HB=200;
							GOD.ListCover(histchapter,VecF,ongrid,Frames);
							HIS.render_T(5, 15,"");
						} else {
							GOD.ListCover(histchapter,VecF);
							B_UP.render_T(580, 5,"");
							B_DOWN.render_T(580, 630,"");
						}
					}
				} else NOP.render_T(230, 402,"");

				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.Comic_16, "history", {100,0,0});
				if(ongrid) {
					gTextTexture.render(5, 1);
				}else {
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
				}

				{//Draw footer buttons
					int dist = 1100,posdist = 180;
					B_A.render_T(dist, 680,"Aceptar"); dist -= posdist;
					B_B.render_T(dist, 680,"Volver"); dist -= posdist;
					B_X.render_T(dist, 680,"Borrar #"+to_string(histchapter+1)); dist -= posdist;

				}
				break;
			}
			case top_s:               {
				if(ongrid) {USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);}
				int histsize=BD["arrays"]["Top"]["link"].size();
				if (histsize > 0) {
					//if (favsize > 30) ongrid=false;
					if (!ongrid) GOD.ListClassic(topchapter,BD["arrays"]["Top"]);
					if (preview)
					{
						if (ongrid) {
							VOX.TickerColor(GOD.HR,100,200, 200);
							GOD.HG=200; GOD.HB=200;
							GOD.ListCover(topchapter,BD["arrays"]["Top"],ongrid,Frames);
							TOP.render_T(5, 15,"");
						} else {
							GOD.ListCover(topchapter,BD["arrays"]["Top"]);
							B_UP.render_T(580, 5,"");
							B_DOWN.render_T(580, 630,"");
						}
					}
				} else NOP.render_T(230, 402,"");

				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.Comic_16, "Top Anime", {100,0,0});
				if(ongrid) {
					gTextTexture.render(5, 1);
				}else {
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
				}

				{//Draw footer buttons
					int dist = 1100,posdist = 180;
					B_A.render_T(dist, 680,"Aceptar"); dist -= posdist;
					B_B.render_T(dist, 680,"Volver"); dist -= posdist;
				}
				break;
			}
			case agregados_s: {
				if(ongrid) {USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);}
				int histsize=BD["arrays"]["Agregados"]["link"].size(); //BD["arrays"]["Agregados"]["link"]
				if (histsize > 0) {
					//if (favsize > 30) ongrid=false;
					if (!ongrid) GOD.ListClassic(agregadosidx,BD["arrays"]["Agregados"]);
					if (preview)
					{
						if (ongrid) {
							VOX.TickerColor(GOD.HB,50,200, 200);
							GOD.HG=100; GOD.HR=100;
							GOD.ListCover(agregadosidx,BD["arrays"]["Agregados"],ongrid,Frames);
							GOD.MapT["ULT"].render_T(5, 15,"");
						} else {
							GOD.ListCover(agregadosidx,BD["arrays"]["Agregados"]);
							B_UP.render_T(580, 5,"");
							B_DOWN.render_T(580, 630,"");
						}
					}
				} else NOP.render_T(230, 402,"");

				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.Comic_16, "Ultimos Animes Agregados", {100,0,0});
				if(ongrid) {
					gTextTexture.render(5, 1);
				}else {
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
				}

				{//Draw footer buttons
					int dist = 1100,posdist = 180;
					B_A.render_T(dist, 680,"Aceptar"); dist -= posdist;
					B_B.render_T(dist, 680,"Volver"); dist -= posdist;
				}
				break;
			}
			case hourglass_s:              {
				if(ongrid) {USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);}

				int histsize=BD["arrays"]["HourGlass"]["link"].size();
				if (histsize > 0) {
					//if (favsize > 30) ongrid=false;
					if (!ongrid) GOD.ListClassic(hourchapter,BD["arrays"]["HourGlass"]);
					if (preview)
					{
						if (ongrid) {
							VOX.TickerColor(GOD.HB,100,200, 200);
							GOD.HR=200; GOD.HG=200;
							GOD.ListCover(hourchapter,BD["arrays"]["HourGlass"],ongrid,Frames);
							HOR.render_T(5, 15,"");
						} else {
							GOD.ListCover(hourchapter,BD["arrays"]["HourGlass"]);
							B_UP.render_T(580, 5,"");
							B_DOWN.render_T(580, 630,"");
						}
					}
				} else NOP.render_T(230, 402,"");

				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.Comic_16, "En Emisión", {100,0,0});
				if(ongrid) {
					gTextTexture.render(5, 1);
				}else {
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
				}

				{//Draw footer buttons
					int dist = 1100,posdist = 180;
					B_A.render_T(dist, 680,"Aceptar"); dist -= posdist;
					B_B.render_T(dist, 680,"Volver"); dist -= posdist;

					dist = B_L.render_T(550,680," "+Wday[WdayG]+" ");
					B_R.render_T(550+dist,680);
				}
				break;
			}
			case download_s:              {
				USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);
				VOX.render_VOX({0,0, 1280, 60},200, 200, 200, 130); //Head
				VOX.render_VOX({16,65, 900, 162}, 210, 210, 210, 115);//Rectangle

				gTextTexture.loadFromRenderedText(GOD.Comic_16, "Descargando Actualmente:", textColor);
				gTextTexture.render(posxbase, posybase+15);

				gTextTexture.loadFromRenderedText(GOD.Arista_30, DownTitle, textColor);
				VOX.render_VOX({17,65, gTextTexture.getWidth()+15, 45}, 210, 210, 210, 155);//Draw title back
				gTextTexture.render(posxbase, posybase + 60);

				if (serverenlace != "Error de descarga") {
					gTextTexture.loadFromRenderedText(GOD.Arista_100, to_string(porcendown) + "\%", textColor);
					gTextTexture.render(posxbase + 280, posybase + 90);

					gTextTexture.loadFromRenderedText(GOD.Comic_16, "Peso estimado: " + to_string((int)(sizeestimated / 1000000)) + "mb.", textColor);
					gTextTexture.render(posxbase, posybase + 160);

					gTextTexture.loadFromRenderedText(GOD.Comic_16, "Usa el HomeBrew PPlay para reproducir el video.", textColor);
					gTextTexture.render(posxbase, posybase + 200);

					if (to_string(porcendown) == "100"&&!isDownloading) {
						//Render red filled quad
						VOX.render_VOX({ posxbase + 198, posybase + 500, 580, 50 }, 255, 255, 255, 195);
						gTextTexture.loadFromRenderedText(GOD.Arista_40, "¡Descarga Completada! Revisa tu SD.", textColor);
						gTextTexture.render(posxbase + 200, posybase + 500);
						if(lcdoff) {lcdoff=false; appletSetLcdBacklightOffEnabled(lcdoff);}
					}else{
						gTextTexture.loadFromRenderedText(GOD.digi_16, "Velocidad: " +speedD+" M/S", textColor);
						VOX.render_VOX({ posxbase, posybase + 180, gTextTexture.getWidth()+6, 20 }, 255, 255, 255, 145);
						gTextTexture.render(posxbase + 2, posybase + 180);
						if (isHandheld) {
							SCREEN.render(1180, 65);
							B_ZR.render_T(550, 680,"Apagar Pantalla");
						}
					}
				} else {
					porcendown=0;
				}

				gTextTexture.loadFromRenderedText(GOD.Comic_16, serverenlace.substr(0,300), {168,0,0});
				gTextTexture.render(posxbase, posybase + 220);

				static int tatic = 850;
				VOX.render_VOX({posxbase-5,posybase + 240, tatic, ((int)BD["arrays"]["downloads"]["log"].size() * 22)+33}, 200, 200, 200, 105);
				gTextTexture.loadFromRenderedText(GOD.digi_16, "Cola De Descarga :", textColor);
				gTextTexture.render(posxbase, posybase+240);
				for (u64 x = 0; x < BD["arrays"]["downloads"]["log"].size(); x++) {
					string descarga = BD["arrays"]["downloads"]["log"][x];
					//NameOfLink(descarga);
					SDL_Color txtColor = textColor;//{ 50, 50, 50 };

					if(descarga.substr(0,3) == "100") txtColor = { 0, 100, 0 };
					if(descarga.substr(0,3) == "Err") txtColor = { 150, 50, 50 };
					if(descarga.substr(0,3) == ">>>") {txtColor = { 0, 0, 0 }; replace(descarga,">>>>",">>"+to_string(porcendown)+"\%");}
					if(descarga.substr(0,3) == "htt") {txtColor = { 100, 100, 100}; NameOfLink(descarga); descarga="En Cola: "+descarga;}

					gTextTexture.loadFromRenderedText(GOD.digi_16, descarga, txtColor);
					if (tatic < gTextTexture.getWidth()) {tatic = gTextTexture.getWidth()+35;}
					gTextTexture.render(posxbase, posybase+260 + ((x) * 22));

				}

				if(isDownloading) B_X.render_T(800, 680,"Cancelar la descarga");
				B_B.render_T(1100, 680,"Volver");
				break;
			}
			}

			//global render
			if(isDownloading&& download_s != statenow) {
				T_D.loadFromRenderedText(GOD.digi_16, ""+DownTitle.substr(0,42)+"... ("+to_string(porcendown)+"\%)", {50,50,50});
				VOX.render_VOX({SCREEN_WIDTH - T_D.getWidth() - 2, 671-T_D.getHeight()+4, T_D.getWidth()+4, T_D.getHeight()-5}, 255, 255, 255, 180);
				T_D.render(SCREEN_WIDTH - T_D.getWidth() - 1, 671-T_D.getHeight());
			}
			if (AppletMode) GOD.PleaseWait("Esta App No funciona en Modo Applet. Instalando NSP, Espere...",false);

			//presice clock cicle 1s
			if (inTimeN(1000,0)) {
				if (Frames>0) {
					static int rest=0;
					printf("Frames %d - FPS: %d \r",Frames,Frames-rest);
					fflush(stdout);
					rest=Frames;
				}
				if (!isConnected) {isConnected=Net::HasConnection();}
			}

			//clock cicle 15s
			if (inTimeN(15000)) {
                //Verificar si esta conectado cada 15s
				isConnected=Net::HasConnection();
                
			}
			if (!isConnected) {
				gTextTexture.loadFromRenderedText(GOD.digi_16, "Sin Internet", {255,0,0});
				VOX.render_VOX({SCREEN_WIDTH - gTextTexture.getWidth() - 8,671-gTextTexture.getHeight(), gTextTexture.getWidth()+4, gTextTexture.getHeight()}, 0, 0, 0, 180);
				gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 671-gTextTexture.getHeight() );
			} else if (ClFlock){
                //CloudFlare protege este sitio asi q lo abrimos en en navegador
                GOD.PleaseWait("CloudFlare Activo, Abriendo Navegador Espere...");
				//Just open the browser
				WebBrowserCloud("https://jkanime.net/");

                reloadmain = Net::Bypass();
                ClFlock = false;
            }
			if (UAG){
                //CloudFlare protege este sitio asi q lo abrimos en en navegador
                GOD.PleaseWait("Obteniendo UA...");
				//Just open the browser
				WebBrowserUA();
                Net::Bypass();
                UAG = false;
            }
			if(programation_s != statenow && isHandheld) {
				BACK.render(SCREEN_WIDTH - USER.getWidth() - BACK.getWidth() - 30, 1);
			}

			GOD.MapT["MUSIC"].render_T(10, 680,"",(Mix_PausedMusic() == 1 || Mix_PlayingMusic() == 0));
/*
			if((programation_s == statenow && isHandheld)|quit) {
				GOD.MapT["EXIT"].render_T(80, 680,"",quit);
			}
*/
			//Display the list
			if (!quit&&!reloading&&!AppletMode&&Frames>2) {preview = true;}

			if (Frames>1000) Frames=0;
			if (Frames>0) Frames++;
			//Update tik
			if (inTimeN(1260000) || reloadmain) {
				if (Net::HasConnection()) {
					if(!isChained) {
                        reloadmain = false;
						cout << "Reloading Animes" << endl;
						//Set main Thread get images and descriptions
						Loaderthread = SDL_CreateThread(AnimeLoader, "Loaderthread", (void*)NULL);
					}
				}
			}
			//HID Events
			//#include "input.hpp"
			//InputHandle();
			checkConfirmar();
			if(quit) {
				//wallpaper
				Farest.render((0), (0));
			}
		}
    } catch(const char* errorMessage) {
        std::cout << "Error: " << errorMessage << std::endl;
    } catch(const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    } catch(...) {
		LOG::E(1);
		cout << "- Error Catched Main" << endl;
		GOD.PleaseWait("A ocurrido un error Critico la app se va a cerrar",true);
		cout << "com: " << BD["com"] << endl;
		write_DB(AB,rootdirectory+"AnimeMeta.json.bak");
		write_DB(BD,rootdirectory+"DataBase.json.bak");
		write_DB(UD,rootdirectory+"UserData.json.bak");
		quit=true;
	}
	//Quit if loop break for no reason
	quit=true;
	cancelcurl=1;
	//clear allocate
	BD["com"] = "{}"_json;

    emmc::Save();
    
    if (CapS.joinable()) {CapS.join();}
    if (CapP.joinable()) {CapP.join();}
	if (NULL == capithread) {printf("capithread Not in use\n");} else {printf("capithread in use: %s\n", SDL_GetError()); SDL_WaitThread(capithread, NULL);}
	if (NULL == downloadthread) {printf("downloadthread Not in use\n");} else {printf("downloadthread in use: %s\n", SDL_GetError()); SDL_WaitThread(downloadthread, NULL);}
	if (NULL == searchthread) {printf("searchthread Not in use\n");} else {printf("searchthread in use: %s\n", SDL_GetError()); SDL_WaitThread(searchthread,NULL);}
	if (NULL == Loaderthread) {printf("Loaderthread Not in use\n");}else {printf("Loaderthread in use: %s\n", SDL_GetError()); SDL_WaitThread(Loaderthread, NULL);}
	if (NULL == inputThread) {printf("inputThread Not in use\n");}else {printf("inputThread in use: %s\n", SDL_GetError()); SDL_WaitThread(inputThread, NULL);}


	//Free loaded images
	gTextTexture.free();
	Farest.free();
	Heart.free();

	//end net before anything
	nifmExit();
	B_A.free();
	B_B.free();
	B_Y.free();
	B_X.free();
	B_L.free();
	B_R.free();
	B_ZR.free();
	B_M.free();
	B_P.free();
	B_RIGHT.free();
	B_LEFT.free();
	B_UP.free();
	B_DOWN.free();
	FAV.free();
	NOP.free();
	BUS.free();
	REC.free();
	USER.free();
	NFAV.free();
	CLEAR.free();
	SCREEN.free();

	//Free resources and close SDL
	GOD.deint();

	//LOG END
    LOG::deinit();


	accountExit();
	hidsysExit();
	socketExit();
	romfsExit();
	socketExit();


	//unmount and commit
	mount_theme("themes",false);

	user::deinitUser();

	emmc::deinit();

	accountExit();
	appletUnlockExit ();
	if (AppletMode) {
		appletRequestLaunchApplication (0x05B9DB505ABBE000, NULL);
	}
	//if (!isConnected) appletRequestToSleep();
	return 0;
}