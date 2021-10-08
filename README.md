# RipJKNX <img style="display:inline" src="Icon.jpg" width="32"/>
 App de [JKAnime](https://jkanime.net/) para Nintendo Switch
----------------------------------------
Esta Aplicación es para Ver Online y Descargar Anime a través de la Nintendo Switch.


<img style="display:inline" src="ScreenShots/ScreenShots_2.jpg" width="232"/> <img style="display:inline" src="ScreenShots/ScreenShots_3.jpg" width="232"/> <img style="display:inline" src="ScreenShots/ScreenShots_4.jpg" width="232"/>

### Para instalarlo Solo deberás de:
* Copiar [RipJKAnime_NX.nro](/out/RipJKAnime_NX.nro?raw=true)  en /switch/ y abrirlo una vez desde el Álbum esto instalara el icono en el Home.
* * Solo debes hacer esto una vez a menos que pierdas el icono del Home 
<details>
  <summary>Metodo Alternativo</summary>
 <li> Copiar el archivo <a href="/out/RipJKAnimeNX[05B9DB505ABBE000][v0].nsp?raw=true">RipJKAnimeNX[05B9DB505ABBE000][v0].nsp</a>  en la SD.</li>
 <li> Instalar el RipJKAnime NX[05B9DB505ABBE000][v0].nsp Con GoldLeaf o Awoo-installer</li>
</details>

## Actualizar la app
* Esta Aplicacion se Actualiza de forma automática para que tengas siempre la ultima versión
* * (A menos que se desactive deliberadamente)
# A tener en cuenta
* Esta app no funciona correctamente en modo applet (desde el album) porque requiere mas memoria y del uso del navegador 
* Debes usar lo básico antibaneo (dns, incógnito ,host de atmosphere,etc)
* Necesita Si o Si Internet 
* Para ver Animes descargados puedes usar [PPlay](https://github.com/Cpasjuste/pplay/) se guardan en la Raíz de la sd /Videos/


<details>
  <summary>Como Compilar, Devs</summary>

# Compilar 
esta app hace uso de [nspmini](https://github.com/StarDustCFW/nspmini) como librería
```sh
# Deberás usar los siguientes comandos para instalar nspmini en devkitpro
# ya q no viene de serie
git clone https://github.com/StarDustCFW/nspmini
make -C nspmini portlib

# Compilar NRO
make

# Compilar NRO y NSP
make NSP

``` 
</details>




<details>
  <summary>ToDo</summary>
 
## ToDo
- [ ] **Gestor de Pieles**
- [ ] **Gestionar la UI de forma mas simple**
- [ ] **Sección de ajustes**
- [ ] **Abrir PPLAY desde la app**
- [ ] **Integrar un reproductor interno**
- [ ] **Que la app no se congele al usar el navegador**
- [ ] **Crear Salvas de Usuario si no existen**
- [ ] **Utilizar OpenGL**
### Done 
- [x] **Agregar slideshow**
- [x] **Agregar sección de recomendados**
- [x] **Gestionar la interfaz de decargas**
- [x] **Cargar luego del vector #30**
- [x] **Agregar Eliminar Cache**
- [x] **Añadir Sección de Programación semanal**
- [x] **Agregar un Historial**
- [x] **Crear un Auto Actualizador**
- [x] **Auto instalar el nsp cuando se actualizá**
- [x] **Hacer una lista de imágenes en lugar de plana**
</details>
