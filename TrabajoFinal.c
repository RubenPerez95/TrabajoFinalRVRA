#include <GL/glut.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

// ==== Definicion de constantes y variables globales ===============
int    patt_id;            // MarcaSimple
int    patt_id_identic;    // Marca de Identic
double patt_trans[3][4];   // Matriz de transformacion de la marca

static GLint rotate = 0;
float incremento_x = 0.0;
float incremento_y = 0.0;
float incremento_z = 0.0;
long hours = 0;          // Horas transcurridas
int sentido = 0;

// ==== Definicion de estructuras ===================================
struct TObject{
  int id;                      // Identificador del patron
  int visible;                 // Es visible el objeto?
  double width;                // Ancho del patron
  double center[2];            // Centro del patron
  double patt_trans[3][4];     // Matriz asociada al patron
  void (* drawme)(void);       // Puntero a funcion drawme
};

struct TObject *objects = NULL;
int nobjects = 0;
int distancia; //Distancia entre objeto 0 y 1

void print_error (char *error) {  printf(error); exit(0); }


void addObject(char *p, double w, double c[2], void (*drawme)(void))
{
  int pattid;

  if((pattid=arLoadPatt(p)) < 0)
    print_error ("Error en carga de patron\n");

  nobjects++;
  objects = (struct TObject *)
    realloc(objects, sizeof(struct TObject)*nobjects);

  objects[nobjects-1].id = pattid;
  objects[nobjects-1].width = w;
  objects[nobjects-1].center[0] = c[0];
  objects[nobjects-1].center[1] = c[1];
  objects[nobjects-1].drawme = drawme;
}



// ======== cleanup =================================================
static void cleanup(void) {
  arVideoCapStop();  arVideoClose();  argCleanup();  free(objects);
  exit(0);
}

// ======== keyboard ================================================
static void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 0x1B: case 'Q': case 'q':
    cleanup(); break;
  }
}

void draw( void ) {
  double  gl_para[16];   // Esta matriz 4x4 es la usada por OpenGL
  GLfloat light_position[]  = {100.0,-200.0,200.0,0.0};
  GLfloat material1[]     = {0.0, 1.0, 0.0, 0.0};
  GLfloat material2[]     = {0.0, 0.0, 1.0, 1.0};
  GLfloat material3[]     = {0.0, 1.0, 1.0, 0.0};
  GLfloat material4[]     = {1.0, 0.0, 0.0, 0.0};
  double m[3][4], m2[3][4];
  int i;
  float angle=0.0, module=0.0;
  double v[3];
  int cambio = 0;
  float RotTierra = 0.0;     // Movimiento de traslacion de la tierra
  //float RotEarthDay = 0.0;  // Movimiento de rotacion de la tierra
  float size = 0.0;            // Tamaño al que debe escalar el objeto




  argDrawMode3D();              // Cambiamos el contexto a 3D
  argDraw3dCamera(0, 0);        // Y la vista de la camara a 3D
  glClear(GL_DEPTH_BUFFER_BIT); // Limpiamos buffer de profundidad
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  if ((objects[0].visible)) {
    argConvGlpara(objects[0].patt_trans, gl_para);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(gl_para);   // Cargamos su matriz de transf.

    if (objects[1].visible) {
       // Calculamos el angulo de rotacion de la segunda marca
      v[0] = objects[1].patt_trans[0][0];
      v[1] = objects[1].patt_trans[1][0];
      v[2] = objects[1].patt_trans[2][0];

      module = sqrt(pow(v[0],2)+pow(v[1],2)+pow(v[2],2));
      v[0] = v[0]/module;  v[1] = v[1]/module; v[2] = v[2]/module;
      distancia = sqrt(pow(m2[0][3],2)+pow(m2[1][3],2)+pow(m2[2][3],2));

      size = ((500 - distancia) / 160.0) * 100;
      printf ("Distancia objects[0] y objects[1]= %G\n", size);
      angle = acos (v[0]) * 57.2958;   // Sexagesimales! * (180/PI)
      cambio = 1;
    } else {
      cambio = 2;
    }

    if (angle >= 0 && angle < 45) {
      glMaterialfv(GL_FRONT, GL_AMBIENT, material1);
    }
    if (angle >= 45 && angle < 90) {
      glMaterialfv(GL_FRONT, GL_AMBIENT, material2);
    }
    if (angle >= 90 && angle < 135) {
      glMaterialfv(GL_FRONT, GL_AMBIENT, material3);
    }
    if (angle >= 135 && angle < 180) {
      glMaterialfv(GL_FRONT, GL_AMBIENT, material4);
    }

    if(cambio == 1) {
      glEnable(GL_LIGHTING);  glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_POSITION, light_position);
      glTranslatef(0.0, 0.0, 60.0);
      glRotatef(90.0, 1.0, 0.0, 0.0);
      glutSolidTeapot(size);
    } else if (cambio == 2) {
      glEnable(GL_LIGHTING);  glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_POSITION, light_position);

      hours++;
      RotTierra = (hours / 24.0) * (360 / 365.0) * 100;  // x10 rapido!
      glMaterialfv(GL_FRONT, GL_AMBIENT, material1);

      // glRotatef (-RotEarth, 0.0, 0.0, 1.0);
      glTranslatef(0,0,100);
      glRotatef (-RotTierra, 0.0, 0.0, 1.0);
      material1[0] = 0.1; material1[1] = 0.1; material1[2] = 1.0;
      glMaterialfv(GL_FRONT, GL_AMBIENT, material1);
      glutWireSphere (50, 25, 25); // Tierra
      // glMatrixMode(GL_MODELVIEW);
      // glPushMatrix();
      // glTranslatef(0.0, 0.0, 60.0); // move back to focus of gluLookAt
      // glRotatef(90.0, 1.0, 0.0, 0.0); //  rotate around center
      // glTranslatef(0.0, 0.0, 60.0); //move object to center
      // glutSolidTorus(20.0, 20.0 , 4, 5);
      // glPopMatrix();
    }
  }
  glDisable(GL_DEPTH_TEST);
}

// ======== init ====================================================
static void init( void ) {
  ARParam  wparam, cparam;   // Parametros intrinsecos de la camara
  int xsize, ysize;          // Tamano del video de camara (pixels)
  double c[2] = {0.0, 0.0};  // Centro de patron (por defecto)


  // Abrimos dispositivo de video
  if(arVideoOpen("-dev=/dev/video1") < 0) exit(0);
  if(arVideoInqSize(&xsize, &ysize) < 0) exit(0);

  // Cargamos los parametros intrinsecos de la camara
  if(arParamLoad("data/camera_para.dat", 1, &wparam) < 0)
    print_error ("Error en carga de parametros de camara\n");

  arParamChangeSize(&wparam, xsize, ysize, &cparam);
  arInitCparam(&cparam);   // Inicializamos la camara con "cparam"

  // Inicializamos la lista de objetos
  addObject("data/identic.patt", 120.0, c, NULL);
  addObject("data/simple.patt", 90.0, c, NULL);

  argInit(&cparam, 1.0, 0, 0, 0, 0);   // Abrimos la ventana
}

// ======== mainLoop ================================================
static void mainLoop(void) {
  ARUint8 *dataPtr;
  ARMarkerInfo *marker_info;
  int marker_num, i, j, k;

  // Capturamos un frame de la camara de video
  if((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
    // Si devuelve NULL es porque no hay un nuevo frame listo
    arUtilSleep(2);  return;  // Dormimos el hilo 2ms y salimos
  }

  argDrawMode2D();
  argDispImage(dataPtr, 0,0);    // Dibujamos lo que ve la camara

  // Detectamos la marca en el frame capturado (return -1 si error)
  if(arDetectMarker(dataPtr, 100, &marker_info, &marker_num) < 0) {
    cleanup(); exit(0);   // Si devolvio -1, salimos del programa!
  }

  arVideoCapNext();      // Frame pintado y analizado... A por otro!

  // Vemos donde detecta el patron con mayor fiabilidad
  for (i=0; i<nobjects; i++) {
    for(j = 0, k = -1; j < marker_num; j++) {
      if(objects[i].id == marker_info[j].id) {
	if (k == -1) k = j;
	else if(marker_info[k].cf < marker_info[j].cf) k = j;
      }
    }
    if(k != -1) {   // Si ha detectado el patron en algun sitio...
      objects[i].visible = 1;
      arGetTransMat(&marker_info[k], objects[i].center,
		    objects[i].width, objects[i].patt_trans);
      draw();           // Dibujamos los objetos de la escena
    } else { objects[i].visible = 0; }  // El objeto no es visible
  }

  argSwapBuffers(); // Cambiamos el buffer con lo que tenga dibujado
}



// ======== Main ====================================================
int main(int argc, char **argv) {
  glutInit(&argc, argv);    // Creamos la ventana OpenGL con Glut
  init();                   // Llamada a nuestra funcion de inicio

  arVideoCapStart();        // Creamos un hilo para captura de video
  argMainLoop( NULL, NULL, mainLoop );   // Asociamos callbacks...
  return (0);
}
