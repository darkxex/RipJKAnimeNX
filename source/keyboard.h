			//User presses a key
			else if (e.type == SDL_KEYDOWN)
			{
				//Select surfaces based on key press
				switch (e.key.keysym.sym)
				{
				case SDLK_DOWN:
					switch (statenow)
					{
					case searchstate:
						if (!reloadingsearch)
						{
							TSearchPreview.free();
							if (searchchapter < (int)arraysearch.size() - 1)
							{
								searchchapter++;

								std::cout << searchchapter << std::endl;
							}
							else {
								searchchapter = 0;
							}
							callimagesearch();
						
						}
						break;

					case programationstate:
						if (!reloading)
						{
							TPreview.free();


							if (selectchapter < (int)arraychapter.size() - 1)
							{
								selectchapter++;

								std::cout << selectchapter << std::endl;
							}
							else {
								selectchapter = 0;
							}

							callimage();
							
						}
						break;

					case chapterstate:

						if (capmore > 10)
						{
							capmore = capmore - 10;
						}
						if (capmore < mincapit)
						{
							capmore = mincapit;
						}
						break;

					case favoritesstate:
						if (favchapter < (int)arrayfavorites.size() - 1)
						{
							favchapter++;

							std::cout << favchapter << std::endl;
						}
						else {
							favchapter = 0;
						}
						break;

					}

					break;

				case SDLK_UP:
					switch (statenow)
					{
					case programationstate:
						if (!reloading)
						{
							TPreview.free();
							if (selectchapter > 0)
							{
								selectchapter--;
								std::cout << selectchapter << std::endl;
							}
							else {
								selectchapter = arraychapter.size() - 1;
							}
							callimage();
							
						}
						
						break;
					case chapterstate:

						if (capmore < maxcapit)
						{
							capmore = capmore + 10;
						}
						if (capmore > maxcapit)
						{
							capmore = maxcapit;
						}
						break;

					case searchstate:
						if (!reloadingsearch)
						{
							TSearchPreview.free();
							if (searchchapter > 0)
							{
								searchchapter--;
								std::cout << searchchapter << std::endl;
							}
							else {
								searchchapter = arraysearch.size() - 1;
							}
							callimagesearch();
							
						}
						break;

					case favoritesstate:
						
						if (favchapter > 0)
						{
							favchapter--;
							std::cout << favchapter << std::endl;
						}
						else {
							favchapter = arrayfavorites.size() - 1;
						}
						break;

					}
					break;
				case SDLK_a:

					switch (statenow)
					{
					case programationstate:

					{if (!reloading&&arraychapter.size()>=1)
					{
						TChapters.free();
						TChapters.loadFromFileCustom(tempimage, 550, 400);
						statenow = chapterstate;
						temporallink = arraychapter[selectchapter];

						int val1 = temporallink.find("/", 20);
						temporallink = temporallink.substr(0, val1 + 1);
						std::cout <<"q wea es esto?"<< temporallink << std::endl;
						maxcapit = atoi(capit(temporallink).c_str());
						if (tienezero == true) {
							maxcapit = maxcapit - 1;
							mincapit = 0;
							capmore = maxcapit;
						}
						else
						{
							mincapit = 1;
							capmore = maxcapit;
						}
						std::cout << maxcapit << std::endl;

					}
					}
					break;

					case searchstate:
					{
						if (!reloadingsearch&&arraysearch.size()>=1)
						{
							TChapters.free();
							TChapters.loadFromFileCustom(tempimage, 550, 400);
							statenow = chapterstate;
							temporallink = arraysearch[searchchapter];

							std::cout << temporallink << std::endl;
							maxcapit = atoi(capit(temporallink).c_str());
							if (tienezero == true) {
								maxcapit = maxcapit - 1;
								mincapit = 0;
								capmore = maxcapit;
							}
							else
							{
								mincapit = 1;
								capmore = maxcapit;
							}
							std::cout << maxcapit << std::endl;
						}

					}
					break;

					case favoritesstate:
					{
						
							TChapters.free();
							TChapters.loadFromFileCustom(tempimage, 550, 400);
							statenow = chapterstate;
							temporallink = arrayfavorites[favchapter];

							std::cout << temporallink << std::endl;
							maxcapit = atoi(capit(temporallink).c_str());
							if (tienezero == true) {
								maxcapit = maxcapit - 1;
								mincapit = 0;
								capmore = maxcapit;
							}
							else
							{
								mincapit = 1;
								capmore = maxcapit;
							}
							std::cout << maxcapit << std::endl;
						

					}
					break;
					case chapterstate:
						std::string tempurl = temporallink + std::to_string(capmore) + "/";
						onlinejkanimevideo(tempurl);
						break;


					}
					break;
				case SDLK_b:

					switch (statenow)
					{
					case programationstate:


						break;
					case downloadstate:
						cancelcurl = 1;
						statenow = chapterstate;


						break;
					case chapterstate:
						cancelcurl = 1;
						txtyase = "";
						switch (returnnow)
						{
						case toprogramation:
							statenow = programationstate;
							break;
						case tosearch:
							statenow = searchstate;
							break;
						}

						break;
					case searchstate:
						if (!reloadingsearch)
						{
							
							returnnow = toprogramation;
							statenow = programationstate;
							TSearchPreview.free();
						}
						break;

					case favoritesstate:
						statenow = programationstate;
						break;


					}
					break;
				case SDLK_y:

					switch (statenow)
					{
					case programationstate:


						break;
					case downloadstate:



						break;
					case chapterstate:
					{//AGREGAR A FAVORITOS
						if(!isFavorite(temporallink)){
						outfile.open(favoritosdirectory, std::ios_base::app); // append instead of overwrite
						outfile << temporallink;
						outfile << "\n";
						outfile.close();
						}
						txtyase = "(Ya se agreg�)";
					}

					break;
					case favoritesstate:

						std::ofstream ofs(favoritosdirectory, std::ofstream::trunc);

						ofs << "";

						ofs.close();
						
						if (!reloading)
						{
							arrayfavorites.clear();
							statenow = favoritesstate;
							std::string temp;
							std::ifstream infile;

							std::ifstream file(favoritosdirectory);
							std::string str;
							while (std::getline(file, str)) {
								std::cout << str << "\n";
								if (str.find("jkanime"))
								{
									arrayfavorites.push_back(str);
								}
							}
							file.close();


						}
					       break;




					}
					break;
				case SDLK_l:

					switch (statenow)
					{
					case programationstate:

						
						break;
					case downloadstate:



						break;
					case chapterstate:


						break;
					case searchstate:


						break;

					}
					break;
				case SDLK_f:

					switch (statenow)
					{
					case programationstate:
						if (!reloading)
						{
							arrayfavorites.clear();
							statenow = favoritesstate;
							std::string temp;
							std::ifstream infile;

							std::ifstream file(favoritosdirectory);
							std::string str;
							while (std::getline(file, str)) {
								std::cout << str << "\n";
								if (str.find("jkanime"))
								{
									arrayfavorites.push_back(str);
								}
							}
							file.close();
							

						}

						break;
					case downloadstate:



						break;
					case chapterstate:


						break;
					case searchstate:


						break;
					case favoritesstate:

						statenow = programationstate;
							break;

					}
					break;
				case SDLK_z:

					switch (statenow)
					{
					case programationstate:
						//refrescar();



					{
						
							preview = true;
							TPreview.free();
							callimage();

						
						

					}
					break;
					case downloadstate:



						break;
					case chapterstate:


						break;
					case searchstate:
						
							preview = true;
							TSearchPreview.free();
							callimagesearch();

					

						break;


					}
					break;
				case SDLK_x:

					switch (statenow)
					{
					case programationstate:


						break;
					case downloadstate:


						break;
					case chapterstate:

						if (ahorro == true)
						{
							ahorro = false;
						}
						else
						{
							ahorro = true;
						}



						break;
					case searchstate:

						break;


					}
					break;
				case SDLK_LEFT:
					switch (statenow)
					{
					case downloadstate:


						break;

					case chapterstate:
						if (capmore > mincapit)
						{
							capmore = capmore - 1;
						}
						if (capmore < mincapit)
						{
							capmore = mincapit;
						}
						break;
					}

					break;
				case SDLK_p:
					if (Mix_PlayingMusic() == 0)
					{
						//Play the music
						Mix_PlayMusic(gMusic, -1);
					}
					//If music is being played
					else
					{
						//If the music is paused
						if (Mix_PausedMusic() == 1)
						{
							//Resume the music
							Mix_ResumeMusic();
						}
						//If the music is playing
						else
						{
							//Pause the music
							Mix_PauseMusic();
						}
					}
					break;
				case SDLK_r:
					switch (statenow)
					{
					case programationstate:
						if (!reloadingsearch)
						{
							searchchapter = 0;

							TSearchPreview.free();
							arraysearch.clear();
							arraysearchimages.clear();
							statenow = searchstate;
							returnnow = tosearch;
							char *buf = (char*)malloc(256);
#ifdef __SWITCH__
							strcpy(buf, Keyboard_GetText("Buscar Anime (3 letras minimo.)", ""));
#endif // SWITCH


							std::string tempbus(buf);
							searchtext = tempbus;

							searchthread = SDL_CreateThread(searchjk, "searchthread", (void*)NULL);

							break;


						}
					}

					break;

				case SDLK_RIGHT:

					switch (statenow)
					{
					case programationstate:

						break;
					case downloadstate:


						break;
					case chapterstate:
						if (capmore < maxcapit)
						{
							capmore = capmore + 1;
						}
						if (capmore > maxcapit)
						{
							capmore = maxcapit;
						}
						break;

					}


					break;

				default:

					break;
				}
			}